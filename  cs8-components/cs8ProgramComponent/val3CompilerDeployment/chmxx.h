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

#ifndef __chm_h__
#define __chm_h__

#include <string>
#include <ostream>
#include <istream>
#include <vector>
#include <list>
#include <map>

/**
 * @mainpage
 * This is a c++ library for accessing the Microsoft(TM)
 * help files (mostly .chm files).<br> For latest updates
 * and more information please see<br>
 *      <ul>http://www.mare.ee/indrek/libchmxx/</ul>
 */


struct chmFile;

/** This is the namespace for libchmxx library.
  * The library should not export any sumbols to global
  * namespace.
  */
namespace chm {

/** This represents a tree of topics with titles and paths. */
struct chm_topics_tree {
  std::string title;  ///< Title of the tree node
  std::string path;   ///< Path to file under given topic or empty
  chm_topics_tree *parent; ///< Pointer to parent tree node, NULL if no parent
  std::list<chm_topics_tree *> children; ///< std::list of children node

  ~chm_topics_tree ();
};

/** Matched document inside the search result. */
struct chm_search_document {
  std::string title;                ///< Document title.
  std::string path;                 ///< Document path.
  std::vector<int> offsets;         ///< Where inside this document the result was found
};

/** Search match for keyword found inside the index. */
struct chm_search_match {
  int is_title;                    ///< Given key is inside a title
  std::string key;                  ///< Key that was found inside the index.
  std::vector<chm_search_document> documents; ///< Documents matching to this index.
};

/** A chm archive. */
class chmfile {
public:
  /** Construct a new chmfile object from given path. */
  chmfile (const std::string& path);
  virtual ~chmfile ();

  /** Tests whether archive is valid and open. */
  bool is_open () const;

  /** Close the archive and free resources. */
  void close ();

  /** Tests whether opened archive is valid. */
  inline operator bool() const { return is_open(); }

  /** Read the entire contents of given file and write to stream out. */
  bool read (const std::string& path, std::ostream& out) const;

  /** Read the entire contents of a file and store inside given vector.
   * The vector is resized to file size. */
  bool read (const std::string& path, std::vector<char>& out) const;

  /** Read buf_size bytes from path and place into buf. */
  bool read (const std::string& path, char *buf, size_t buf_size) const;

  /** Returns the size of given file inside archive, returns 0 if file does not exist. */
  std::streamsize file_size (const std::string& path) const;

  /** Returns a boolean value whether given file exists inside the archive. */
  bool file_exists (const std::string& path) const;

  /** Use a combination of these as argument to the readdir. */
  enum readdir_type { files = 1, dirs = 2, special = 4, meta = 8 };

  /** Read entries under given path and append them to the list.
    * Files of directory type will have '/' at the end of their names. */
  bool readdir (const std::string& path, std::list<std::string>& entries, int type = files|dirs) const;

  /** Open given path inside the archive for reading and return a streambuf object.
    * Returns 0 in case file does not exist or an error happened. You're
    * better off to using chmistream than this method. The returned object should
    * be deleted after use. */
  std::streambuf* open (const std::string& path, size_t buf_size = 1024) const;

  inline const std::string& get_title () const { return title; }
  inline const std::string& get_home_file () const { return home_file; }
  inline const std::string& get_generator () const { return generator; }
  inline const std::string& get_index_file () const { return index_file; }
  inline const std::string& get_topics_file () const { return topics_file; }
  inline const std::string& get_path () const { return path; }

  /** Gets the topics tree. If this is called first time the tree
    * is calculated and stored for later use. In case the chm file
    * has no soptics tree NULL is returned. */
  const chm_topics_tree * get_topics_tree () const;

  /** Search the archive for given keyword and return found entries in found. */
  bool search_index (const std::string& txt, std::list<chm_search_match>& found,
      bool whole_words = true, bool titles_only = true) const;

  /** Cache given file for _fast_ access inside memory. All streams opened
    * for that file will be memory cached. I know this caching business
    * is a bit superfluous. It's not all good too - if you start
    * unreading stuff using the stream the cache will be changed.
    */
  bool cache (const std::string& path);
  /** Cache all metadata that is required to perform fast searches inside the index. */
  void cache_search_database ();

private:
  mutable chmFile *chm;
  std::string path;

  std::string title, home_file, generator, index_file, topics_file;

  mutable chm_topics_tree *tree;

  chmfile (const chmfile&);
  chmfile& operator= (const chmfile&);

  typedef std::map<std::string,std::vector<char> > cache_data_t;
  cache_data_t cache_data;
};

/** Inputstream for reading files inside a chm archives.
  * @code
        chm::chmfile chm("/chmfile.chm");
        chm::chmistream in(chm, "/path");
        string line;
        while ( readline(chm, line) ) {
            cout << line;
        }
  * @endcode
  */
class chmistream : public std::istream {
public:
  /** Create a new input stream from archive for given path. */
  chmistream (const chmfile& chm, const std::string& path, size_t buf_size = 1024);

  /** Returns the number of bytes that are left to be read from this stream.
   * So returns the size of file when nothing has been read. */
  std::streamsize read_left () const;

  virtual ~chmistream ();

  /** Get little endian MS encint from the stream. */
  inline size_t get_encint()
  {
      size_t result = 0;
      int shift = 0;

      while (1) {
          int n = get();
          result |= (n & 0x7f) << shift;
          shift += 7;
          if ( !(n & 0x80) ) break;
      };

      return result;
  }

  /** Get local endianized MS dword (2 x 2 bytes) from the stream. */
  inline unsigned long get_dword ()
  {
    char buf[4];
    read (buf, 4);
    size_t res = 0;
    res |= (unsigned char)buf[0] << 0;
    res |= (unsigned char)buf[1] << 8;
    res |= (unsigned char)buf[2] << 16;
    res |= (unsigned char)buf[3] << 24;
    return res;
  }

  /** Get local endianized MS word (2 bytes) from the stream.
    */
  inline unsigned int get_word ()
  {
    char buf[2];
    read (buf, 2);
    unsigned int res = 0;
    res |= (unsigned char)buf[0] << 0;
    res |= (unsigned char)buf[1] << 8;
    return res;
  }

  /** Get a s-r encoded value from the stream. Scale must always be 2.
    * Use 0 for initial pos. When finished reading an entry of sr-s
    * call the get_sr_finish(pos).*/
  inline unsigned long get_sr (int /*s*/, int r, int& pos)
  {
    int b = peek();
    int p = 0;
    while ( (1 << pos) & b ) {
        p++;
        pos++;
        if ( pos == 8 ) {
            get();
            b = peek ();
            pos = 0;
        }
    }

    pos++;    // skip the 0 of p
    if ( pos == 8 ) {
        get ();
        b = peek ();
        pos = 0;
    }

    if ( p > 1 ) r += p - 1; // r now has number of bits for q

    unsigned long res = 0;

    for ( int i = 0; i < r; i++ ) {
        res |= ((1 << pos) & b) ? (1 << i) : 0;
        pos++;
        if ( pos == 8 ) {
            get ();
            b = peek ();
            pos = 0;
        }
    }

    res |= 1 << r;

    return res;
  }

  inline void get_sr_finish (int &pos)
  {
    if ( pos ) {
        get();
        pos = 0;
    }
  }

private:
  std::streambuf *buf;
  bool release;
};

}

#endif // __chm_h__

