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

#include <algorithm>
#include <iostream>
#include <cctype>

#include "tagreader.h"

using namespace std;

chm::tagreader::tagreader (const string& data) : data(data), pos(this->data.begin()), level(0)
{
}

chm::tagreader::tagstruct chm::tagreader::get_next ()
{
  tagstruct ret;

  ret.total_level = level;

  if ( pos == data.end() ) return ret;

  string::iterator s = find (pos, data.end(), '<');
  if ( s != pos ) {
      ret.text = string(pos, s);
      pos = s;
      return ret;
  }

  bool iq = false;
  string::iterator e = s;
  while ( e != data.end() ) {
//      if ( *e == '"' ) iq = !iq; // <> is not allowed inside strings
      if ( *e == '>' && !iq ) break;
      e++;
  }

  if ( e == data.end() ) {  // broken file
    pos = e;
    return ret;
  }

  pos = e + 1;

  s++;

  // now the tag content is inside [s,e)

  if ( *s == '!' ) return get_next(); // we have a comment or metadata, skip it
  if ( *s == '?' ) return get_next(); // we have speical data, skip it

  if ( *s == '/' ) {  // we have a closed tag
    string str = string(s, e);
    std::transform (str.begin(), str.end(), str.begin(), (int(*)(int))std::tolower);
    str = string (str.begin(), find_if(str.begin(), str.end(), (int(*)(int))std::isspace));  // trim
    ret.tag = str;
    ret.total_level = --level;
    ret.tag_level = --tag_levels[ret.tag.substr(1)];
    return ret;
  }

  // we have an open tag, sheesh, lets process it
  string::iterator it = find_if (s, e, (int(*)(int))std::isspace);
  ret.tag = string (s, it);
  transform (ret.tag.begin(), ret.tag.end(), ret.tag.begin(), (int(*)(int))std::tolower);
  s = it;

  ret.tag_level = ++tag_levels[ret.tag];

  // now read the tag parameters
  while ( s != e ) {
      while ( s != e && isspace (*s) ) s++; // skip whitespace
      if ( s == e ) break;

      string elem;
      while ( s != e && *s != '=' && !isspace (*s) ) elem.push_back(*s++);
//      cerr << "ELEM: " << elem << "\n";
      transform (elem.begin(), elem.end(), elem.begin(), (int(*)(int))std::tolower);
      if ( s != e ) {
          if ( isspace (*s) ) { // just a single tag name
              ret.elems[elem] = "";
              while ( s != e && isspace (*s) ) s++;
              continue;
          }

          if ( *s == '=' ) {  // have element with value
              s++;
              string value;
              if ( *s == '"' ) {
                  s++;
                  while ( s != e && *s != '"' ) value.push_back(*s++);
              } else {
                  while ( s != e && !isspace (*s) ) value.push_back(*s++);
              }
              if ( *s == '"' ) s++;
              ret.elems[elem] = value;
              continue;
          }
      } else {  // end of tag, just element value
          ret.elems[elem] = "";
          break;
      }
  }

  return ret;
}

bool chm::tagreader::skip_to (const std::string& tag_name)
{
  while ( has_next() ) {
      tagstruct n = get_next ();
      if ( n.tag == tag_name ) return true;
  }

  return false;
}

