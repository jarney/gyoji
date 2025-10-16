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
#include <gyoji-misc/xml.hpp>
#include <gyoji-misc/test.hpp>

using namespace Gyoji::misc;

int main(int argc, char **argv)
{
    printf("Testing xml cdata conversions\n");
    
    ASSERT(std::string("<![CDATA[This is a ]]]]><![CDATA[> new string]]>"), xml_to_cdata("This is a ]]> new string"), "Two strings with cdata in the middle");
    ASSERT(std::string("<![CDATA[This is a ]]]]><![CDATA[> new string]]]]><![CDATA[>]]>"), xml_to_cdata("This is a ]]> new string]]>"), "Two strings with cdata at the very end");
    ASSERT(std::string("<![CDATA[This is a test]]>"), xml_to_cdata("This is a test"), "Single string with no cdata delimiters");
    
    ASSERT(std::string("this&quot; is a &quot; good &quot; thing"), xml_escape_attribute("this\" is a \" good \" thing"), "Testing escape of quote character");
    
    printf("    PASSED\n");
}
