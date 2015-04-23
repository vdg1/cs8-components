//
// libchmxx - c++ interface for chmlib
// Copyright (C) 2003, 2008 by Indrek Mandre <indrek@mare.ee>
// http://www.mare.ee/indrek/libchmxx/
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

#include <iostream>
#include <cstdio>
#include <cstring>
#include <streambuf>
#include <map>
#include <algorithm>
#include <cassert>
#include <cctype>

#include "chmxx.h"
#include "chm_lib.h"
#include "tagreader.h"

using std::string;
using std::vector;
using std::list;
using std::map;
using std::find;
using std::transform;
using namespace chm;

namespace {
  class chmstreambuf : public std::streambuf {
  public:
    chmstreambuf (chmFile *chm, chmUnitInfo ui, size_t buf_size = 1024) : chm(chm), ui(ui), addr(0), buf_size(buf_size)
    {
      buf = new char[buf_size];
      is_cached = false;
      file_size = ui.length;

      setg (buf, buf + buf_size, buf + buf_size);
      if ( buf_size == file_size ) underflow(); // force loading of entire buffer, no more loads are done
    }

    chmstreambuf (const char *b, size_t buf_size) : buf((char *)b), buf_size(buf_size)
    {
      is_cached = true;
      setg (buf, buf, buf + buf_size);
      addr = buf_size;
      file_size = buf_size;
    }

    virtual ~chmstreambuf ()
    {
      if ( !is_cached ) delete [] buf;
    }

    virtual pos_type seekoff (off_type amount, std::ios_base::seekdir dir, std::ios_base::openmode)
    {
      LONGUINT64 naddr;

      if (dir == std::ios_base::beg ) {
          naddr = amount;
      } else if ( dir == std::ios_base::cur ) {
          LONGUINT64 cur_addr = addr - (egptr() - gptr()); // determine current real read position
          naddr = cur_addr + amount;
          if ( amount == 0 ) { return naddr; } // tellg optimization
      } else if ( dir == std::ios_base::end ) {
          naddr = file_size + amount;
      } else {
          return pos_type(off_type(-1));
      }

      if ( naddr >= file_size )
          return pos_type(off_type(-1));

      // try to optimise if the seek is inside our buffer
      LONGUINT64 low = addr - (egptr() - eback());
      LONGUINT64 high = addr;

      if ( naddr >= low && naddr < high ) {
          setg (eback(), egptr() - (addr - naddr), egptr());
          return naddr;
      }

      setg (buf, buf + buf_size, buf + buf_size);
      addr = naddr;

      return addr;
    }

    virtual pos_type seekpos (pos_type pos, std::ios_base::openmode om)
    {
      return seekoff (pos, std::ios_base::beg, om);
    }

    virtual std::streamsize showmanyc ()
    {
      return (egptr() - gptr()) + (file_size - addr);
    }

    std::streamsize read_left () const
    {
      return (egptr() - gptr()) + (file_size - addr);
    }

    std::streamsize xsgetn(char_type* __s, std::streamsize __n)
    {
      std::streamsize r = 0;

      if ( gptr() < egptr() ) {
          if ( egptr() - gptr() > __n ) {
              std::memcpy (__s, gptr(), __n);
              gbump (__n);
              return __n;
          } else {
              r = egptr() - gptr();
              std::memcpy (__s, gptr(), r);
          }
      }

      LONGUINT64 m = chm_retrieve_object (chm, &ui, (unsigned char *)(__s + r), addr, __n - r);
      addr += m;

      setg (buf, buf + buf_size, buf + buf_size);

      return r + m;
    }

    virtual int_type underflow ()
    {
      if ( gptr() < egptr () )
          return (unsigned char)*gptr();

      if ( addr >= file_size ) return traits_type::eof();

      std::streamsize r = xsgetn (buf, buf_size);

      setg (buf, buf, buf + r);

      if ( r == 0 )
          return traits_type::eof();

      return (unsigned char)*gptr();
    }

  private:
    chmFile *chm;
    chmUnitInfo ui;
    LONGUINT64 addr;
    char *buf;
    size_t buf_size;
    size_t file_size;
    bool is_cached;
  };
} // namespace {

chm::chmfile::chmfile (const string& path) : path(path)
{
  chm = chm_open (path.c_str());
  tree = NULL;

  if ( !chm ) return;

  chmistream in(*this, "/#SYSTEM");

  if ( in ) {
      in.get_dword();

      while ( in.read_left() ) {
        size_t type = in.get_word();
        size_t len = in.get_word();
        vector<char> data(len);
        in.read (&data[0], len);
//        printf ("type = %ld, len = %ld, data = [%s]\n", type, len, &data[0]);
        switch (type) {
          case 0:
            topics_file = "/" + string(data.begin(), data.end() - 1);
            break;
          case 1:
            index_file = "/" + string(data.begin(), data.end() - 1);
            break;
          case 2:
            home_file = "/" + string(data.begin(), data.end() - 1);
            break;
          case 3:
            title = string(data.begin(), data.end() - 1);
            break;
          case 6:
            if ( topics_file.empty() )
              {
                string t = string(data.begin(), data.end() - 1);
                if ( file_exists (t + ".hhc") )
                    topics_file = "/" + t + ".hhc";
                if ( file_exists (t + ".hhk") )
                    index_file = "/" + t + ".hhk";
              }
            break;
          case 9:
            generator = string(data.begin(), data.end() - 1);
            break;
        }
      }
  }

  if ( topics_file.empty() && index_file.empty() ) {
      vector<char> buf;
      if ( read ("/#STRINGS", buf) && buf.size() ) {
        vector<char>::iterator i = buf.begin();
        i++;  // skip first byte;

        while ( i != buf.end() ) {
          string next = string(i, std::find (i, buf.end(), 0));
          if ( next.size() > 4 ) {
            string prefix = next.substr(next.size() - 4);
            std::transform (prefix.begin(), prefix.end(), prefix.begin(), (int(*)(int))std::tolower);
//            std::cout << next << " " << prefix << std::endl;
            if ( prefix == ".hhc" )
                topics_file = "/" + next;
            else if ( prefix == ".hhk" )
                index_file = "/" + next;
          }

          i += next.size() + 1;
        }
      }
  }
}

const chm::chm_topics_tree * chm::chmfile::get_topics_tree () const
{
  if ( tree || topics_file.empty() ) return tree;

  vector<char> buf;
  if ( read (topics_file, buf) ) {
      tree = new chm_topics_tree ();
      tree->parent = NULL;
      chm_topics_tree *cur = tree;
      chm_topics_tree *last = tree;

      string str (buf.begin(), buf.end());
      buf.clear(); // release memory
  //        std::cout << str << std::endl;
      tagreader tr(str);
//      std::cerr << "Topics tree size in bytes: " << str.size() << "\n";

      while ( tr.has_next() ) {
          tagreader::tagstruct s = tr.get_next();

          if ( s.tag == "object" && s.elems["type"] == "text/sitemap" ) {
            s = tr.get_next ();
            last = new chm_topics_tree ();
            last->parent = cur;
            cur->children.push_back (last);
            while ( s.tag != "/object" ) {
                if ( s.tag == "param" ) {
                    string name = s.elems["name"];
                    string value = s.elems["value"];
                    if ( name == "Name" ) {
                        last->title = value;
                    } else if ( name == "Local" || name == "local" ) {
                        last->path = "/" + value;
                    }
                }
                if ( !tr.has_next () ) break; // faulty file
                s = tr.get_next();

            }
            continue;
          }

          if ( s.tag == "ul" && s.tag_level > 1 ) {
            cur = last;
            continue;
          }

          if ( s.tag == "/ul" && s.tag_level > 0 && cur->parent ) {
            cur = cur->parent;
            continue;
          }
      }
  }

  return tree;
}

bool chm::chmfile::is_open () const
{
  return chm != NULL;
}

std::streambuf* chm::chmfile::open (const string& path, size_t buf_size) const
{
  if ( !cache_data.empty() ) {  // check cache data first
      cache_data_t::const_iterator it = cache_data.find(path);
      if ( it != cache_data.end() )
          return new chmstreambuf (&it->second[0], it->second.size());
  }

  struct chmUnitInfo ui;
  if ( chm_resolve_object (chm, path.c_str(), &ui) == CHM_RESOLVE_FAILURE )
      return NULL;

  return new chmstreambuf(chm, ui, buf_size);
}

std::streamsize chm::chmfile::file_size (const string& path) const
{
  struct chmUnitInfo ui;
  if ( chm_resolve_object (chm, path.c_str(), &ui) == CHM_RESOLVE_FAILURE )
      return 0;
  else
      return ui.length;
}

bool chm::chmfile::file_exists (const string& path) const
{
  struct chmUnitInfo ui;
  return chm_resolve_object (chm, path.c_str(), &ui) != CHM_RESOLVE_FAILURE;
}

bool chm::chmfile::read (const string& path, std::ostream& out) const
{
  struct chmUnitInfo ui;
  if ( chm_resolve_object (chm, path.c_str(), &ui) == CHM_RESOLVE_FAILURE )
      return false;

  chmstreambuf buf(chm, ui);
  out << &buf;

  return true;
}

bool chm::chmfile::read (const string& path, std::vector<char>& out) const
{
  chmistream in(*this, path);
  if ( !in ) return false;
  out.resize (in.read_left());
  in.read (&out[0], out.size());

  return true;
}

bool chm::chmfile::read (const string& path, char *buf, size_t buf_len) const
{
  chmistream in(*this, path);
  if ( !in ) return false;
  in.read (buf, buf_len);

  return true;
}

namespace {
  struct readdir_str {
    std::map<string, int> found;
    std::list<string> *entries;
    string path;
    int flags;
  };

  // this chmlib enumeration is cimpletely screwed up
  int chm_readdir (struct chmFile *, struct chmUnitInfo *ui, void *context)
  {
    readdir_str& ctx = *(readdir_str *)context;
    std::list<string> &entries = *ctx.entries;
    string add = ui->path;
    add = add.substr(ctx.path.size());

    string::iterator it = find(add.begin(), add.end(), '/');
    if ( it != add.end() ) {
        if ( !(ctx.flags & CHM_ENUMERATE_DIRS) )
            return CHM_ENUMERATOR_CONTINUE;
        add.erase (++it, add.end());
    } else if ( !(ctx.flags & CHM_ENUMERATE_FILES) ) {
        return CHM_ENUMERATOR_CONTINUE;
    }

    if ( !ctx.found[add]++ )
        entries.push_back (add);
    return CHM_ENUMERATOR_CONTINUE;
  }
}

// the chmlib-s enumeration seems to be a bit broken and is built
// not very nicely for doing things like this
// for some archives i didn't get the whole directory tree
bool chm::chmfile::readdir (const string& path, std::list<string>& entries, int type ) const
{
  int flags = 0;
  flags |= (type&files) ? CHM_ENUMERATE_FILES : 0;
  flags |= (type&dirs) ? CHM_ENUMERATE_DIRS : 0;
  flags |= (type&special) ? CHM_ENUMERATE_SPECIAL : 0;
  flags |= (type&meta) ? CHM_ENUMERATE_META : 0;
  flags |= CHM_ENUMERATE_NORMAL;

  readdir_str str;
  str.entries = &entries;
  str.path = path;
  str.flags = flags;
  if ( !path.empty() && str.path[str.path.size() - 1] != '/' ) str.path += "/";

  chm_enumerate_dir (chm, path.c_str(), flags, chm_readdir, &str);

  return true;
}

void chm::chmfile::close ()
{
  chm_close (chm);
  chm = NULL;
  delete tree;
  tree = NULL;
}

chm::chmfile::~chmfile ()
{
  if ( is_open () ) close ();
}

namespace {
  template<class T> void destroy (T* obj) { delete obj; }
}

chm::chm_topics_tree::~chm_topics_tree ()
{
  std::for_each (children.begin(), children.end(), destroy<chm_topics_tree>);
}

chm::chmistream::chmistream (const chmfile& chm, const std::string& path, size_t buf_size) : std::istream (NULL)
{
  buf = chm.open (path, buf_size);
  init (buf);
  release = true;
}

chm::chmistream::~chmistream ()
{
  if ( release ) delete buf;
}

std::streamsize chm::chmistream::read_left () const
{
  return ((chmstreambuf *)buf)->read_left ();
}

namespace {
  struct ss_hdr {
    unsigned char dis;
    unsigned char dir;
    unsigned char ccs;
    unsigned char ccr;
    unsigned char lcs;
    unsigned char lcr;
  };

  // should make something more efficient later
  int icmp (const vector<char>& l, const vector<char>&r) {
      string ls(l.begin(), l.end());
      string rs(r.begin(), r.end());
      transform (ls.begin(), ls.end(), ls.begin(), (int(*)(int))std::tolower);
      transform (rs.begin(), rs.end(), rs.begin(), (int(*)(int))std::tolower);
//      if ( ls > rs ) { std::cerr << "ls > rs: , ls = [" << ls << "], rs = [" << rs << "]" << std::endl; }
      if ( ls < rs ) return -1;
      else if ( ls > rs ) return 1;
      else return 0;
  }
}


bool chm::chmfile::search_index (const std::string& text, std::list<chm_search_match>& found,
    bool whole_words, bool titles_only) const
{
  chmistream imain(*this, "/$FIftiMain");
  chmistream iwlc(*this, "/$FIftiMain");
  chmistream itopics(*this, "/#TOPICS");
  chmistream istrings(*this, "/#STRINGS");
  chmistream iurltbl(*this, "/#URLTBL");
  chmistream iurlstr(*this, "/#URLSTR");

  vector<char> search_text(text.begin(), text.end());

  if ( !imain || !itopics || !istrings || !iurltbl || !iurlstr ) return false;

  char hdr[0x32];
  if ( !imain.read(hdr, 0x32) ) return false;

  ss_hdr s;
  memcpy (&s, hdr + 0x1e, 6);

  // scales are always two, nobody ever seen other scales
  // or has credible info how it would work, so scales are kinda pointless
  if ( s.dis != 2 || s.ccs != 2 || s.lcs != 2 ) return false;

  imain.seekg(0x14);
  long node_offset = imain.get_dword();
  long tree_depth = imain.get_word();
  imain.seekg(0x2e);
  long node_len = imain.get_dword();


//  std::cerr << "no: " << node_offset << " nl: " << node_len << " td: " << tree_depth << std::endl;

  vector<char> rw(256);
  vector<char> key;

  while ( node_offset ) {
      if ( tree_depth ) tree_depth--;

      imain.seekg (node_offset);
      if ( imain.read_left() < node_len ) return false;

      long end;

      if ( tree_depth ) { // we're at index node
          long free_space = imain.get_word();
          end = node_offset + node_len - free_space;
      } else {
          long next_offset = imain.get_dword(); // offset
          imain.get_word(); // unknown
          long free_space = imain.get_word();
          end = node_offset + node_len - free_space;
          node_offset = next_offset;
      }

      while ( imain.tellg() < end ) { // read as many entries as there are in this node
          size_t word_len = imain.get() - 1;
          size_t pos = imain.get();

          rw.resize (word_len);
          imain.read(&rw[0], word_len);

          if ( pos && key.size() >= pos ) {
              key.resize(pos);
              key.insert (key.end(), rw.begin(), rw.end());
          } else {
              key = rw;
          }

//          std::cerr << " key = " << string(key.begin(), key.end()) << std::endl;

          if ( tree_depth ) { // index node
              long next_offset = imain.get_dword();
//              std::cerr << "HMM, depth = " << tree_depth << ", next offset = " << next_offset << std::endl;
              imain.get_word();
              if ( icmp (search_text, key) < 0 ) {
                  node_offset = next_offset;
                  break;
              }
              continue;
          }

          // now we have to deal with leaf node
          int is_title = imain.get(); // 0 = body tag, 1 = title tag
          size_t wlc_count = imain.get_encint();
          size_t wlc_offset = imain.get_dword();
          imain.get_word(); // unknown
          imain.get_encint(); // wlc_size

//          std::cerr << "Processing to selecting and filtering and comparing.. " << std::endl;

          if ( !is_title && titles_only || key.size() < search_text.size() ) continue; // we only want titles

          int cmp;

          if ( whole_words ) {
              cmp = icmp (search_text, key);
          } else {
              cmp = icmp (search_text, vector<char>(key.begin(), key.begin() + search_text.size()));
          }

          if ( cmp < 0 ) {
//              std::cerr << "THE END!" << std::endl;
              node_offset = 0;
              break;
          }

          if ( cmp != 0 ) continue; // better luck next time

          found.push_back(chm_search_match());
          chm_search_match &match = found.back();

          match.is_title = is_title;
          match.key = string(key.begin(), key.end());
          match.documents.reserve (wlc_count);

          iwlc.seekg (wlc_offset);
          size_t document_index = 0;

          for ( size_t i = 0; i < wlc_count; i++ ) {
              int pos = 0;
              document_index += iwlc.get_sr (s.dis, s.dir, pos);

              match.documents.push_back(chm_search_document());
              chm_search_document &doc = match.documents.back();

              itopics.seekg(document_index * 16);
              itopics.get_dword(); // offset to #TOCIDX
              int title_offset = itopics.get_dword (); // offset to #STRINGS for title or -1
              int urltbl_offset = itopics.get_dword (); // offset to #URLTBL

              iurltbl.seekg (urltbl_offset);
              iurltbl.get_dword(); // unknown
              iurltbl.get_dword(); // index in #TOPICS
              int urlstr_offset = iurltbl.get_dword();

              iurlstr.seekg(urlstr_offset + 8);
              std::getline (iurlstr, doc.path, '\0');
              doc.path = "/" + doc.path;

              if ( title_offset != -1 ) {
                  istrings.seekg(title_offset);
                  std::getline (istrings, doc.title, '\0');
              }

              size_t count = iwlc.get_sr(s.ccs, s.ccr, pos);
              doc.offsets.reserve(count);

              for ( size_t j = 0; j < count; j++ ) {
                  size_t where = iwlc.get_sr(s.lcs, s.lcr, pos);
                  doc.offsets.push_back(where);
              }

              iwlc.get_sr_finish(pos);
          }
      }
  }

  return true;
}

bool chm::chmfile::cache (const string& path)
{
  if ( cache_data.find(path) != cache_data.end() ) return false; // already in the cache
  chmistream in(*this, path);
  if ( !in ) return false;
  vector<char>& v = cache_data[path];
  v.resize (in.read_left());
  in.read(&v[0], v.size());
  return true;
}

void chm::chmfile::cache_search_database ()
{
  cache("/$FIftiMain");
  cache("/#TOPICS");
  cache("/#STRINGS");
  cache("/#URLTBL");
  cache("/#URLSTR");
}
