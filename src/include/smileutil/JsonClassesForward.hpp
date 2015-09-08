/*F***************************************************************************
 * openSMILE - the open-Source Multimedia Interpretation by Large-scale
 * feature Extraction toolkit
 * 
 * (c) 2008-2011, Florian Eyben, Martin Woellmer, Bjoern Schuller: TUM-MMK
 * 
 * (c) 2012-2013, Florian Eyben, Felix Weninger, Bjoern Schuller: TUM-MMK
 * 
 * (c) 2013-2014 audEERING UG, haftungsbeschränkt. All rights reserved.
 * 
 * Any form of commercial use and redistribution is prohibited, unless another
 * agreement between you and audEERING exists. See the file LICENSE.txt in the
 * top level source directory for details on your usage rights, copying, and
 * licensing conditions.
 * 
 * See the file CREDITS in the top level directory for information on authors
 * and contributors. 
 ***************************************************************************E*/

#ifndef SMILEUTIL_JSONCLASSESFORWARD_HPP
#define SMILEUTIL_JSONCLASSESFORWARD_HPP

namespace smileutil {
namespace json {

    class JsonValue;
    class JsonAllocator;
    class JsonDocument;

    int safeJsonGetInt(const JsonValue &val, const char *name);
    
    template <typename T>
    T checkedJsonGet(const JsonDocument &jsonDoc, const char *varName);

}  // namespace json
}  // namespace smileutil


#endif // SMILEUTIL_JSONCLASSESFORWARD_HPP
