#ifndef _error_hpp
#define _error_hpp

#include <string>

namespace sakura{
    class exception{
    protected:
        const std::string veriant;
        std::string detail;

    public:
        exception () {}
        exception (const exception& ec)
            :veriant(ec.veriant), detail(ec.detail) {}
        exception (const std::string& _veriant, const std::string& _detail)
            :veriant(_veriant), detail(_detail) {}
    };
    
    class index_out_of_bound: public exception {
    public:
        index_out_of_bound (const std::string _detail)
            :exception("Index out of bound : ", _detail) {}
    };

    class runtime_error: public exception {
    public:    
        runtime_error (const std::string _detail)
            :exception("Runtime error : ", _detail) {}    
    };

    class invalid_iterator: public exception {
    public:
        invalid_iterator (const std::string _detail)
            :exception("Invalid iterator : ", _detail) {}    
    };

    class container_is_empty: public exception {
    public:    
         container_is_empty (const std::string _detail)
            :exception("Container is empty : ", _detail) {}    
    };
}

namespace sjtu{
    using namespace sakura;
}

#endif
