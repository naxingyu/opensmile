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

#ifndef SMILEUTIL_JSONCLASSES_HPP
#define SMILEUTIL_JSONCLASSES_HPP

#include <smileutil/JsonClassesForward.hpp>
#include <rapidjson/document.h>

namespace smileutil {
namespace json {
    class JsonValue
    {
    private:
        mutable rapidjson::Value *_p;

    public:
        JsonValue()
            : _p(NULL)
        {
        }

        JsonValue(rapidjson::Value *p)
            : _p(p)
        {
        }

        bool isValid() const
        {
            return (_p != NULL);
        }

        rapidjson::Value* operator-> () const
        {
            return _p;
        }

        rapidjson::Value& operator* () const
        {
            return *_p;
        }
    };

    class JsonAllocator
    {
    private:
        mutable rapidjson::MemoryPoolAllocator<> *_p;

    public:
        JsonAllocator(rapidjson::MemoryPoolAllocator<> *p)
            : _p(p)
        {
        }

        rapidjson::MemoryPoolAllocator<>* operator-> () const
        {
            return _p;
        }

        operator rapidjson::MemoryPoolAllocator<>& () const
        {
            return *_p;
        }
    };

    class JsonDocument
    {
    private:
        mutable rapidjson::Document *_p;

    public:
        JsonDocument(rapidjson::Document *p)
            : _p(p)
        {
        }

        JsonDocument(rapidjson::Document &p)
            : _p(&p)
        {
        }

        rapidjson::Document* operator-> () const
        {
            return _p;
        }

        rapidjson::Document& operator* () const
        {
            return *_p;
        }

        operator rapidjson::Document& () const
        {
            return *_p;
        }
    };

}  // namespace json
}  // namespace smileutil


#endif // SMILEUTIL_JSONCLASSES_HPP
