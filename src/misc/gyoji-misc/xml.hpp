/* Copyright 2025 Jonathan S. Arney
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      https://github.com/jarney/gyoji/blob/master/LICENSE
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
#pragma once

#include <string>

namespace Gyoji::misc {
    
    // Hacks because we don't have a 'proper' xml library.
    std::string xml_to_cdata(const std::string & str);
    std::string xml_escape_attribute(const std::string & str);
    std::string xml_escape_whitespace(const std::string & str);
    
};
