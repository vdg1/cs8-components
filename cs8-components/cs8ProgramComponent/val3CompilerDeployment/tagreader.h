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

#ifndef __tagreader_h__
#define __tagreader_h__

#include <string>
#include <map>

namespace chm {

/** @internal */
class tagreader {
public:
  struct tagstruct {
    std::string tag;
    std::string text;
    std::map<std::string, std::string> elems;
    int tag_level;
    int total_level;
  };
  tagreader (const std::string& data);

  tagstruct get_next ();
  bool has_next () const { return pos != data.end(); }
  bool skip_to (const std::string& tagname);

private:
  std::string data;
  std::string::iterator pos;
  size_t level;
  std::map<std::string, int> tag_levels;
};

}

#endif // __tagreader_h__

