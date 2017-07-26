#ifndef BUNDLE_H
#define BUNDLE_H

#include <string>
namespace _ESBManager
{
    class Bundle
    {
        public:
            Bundle();
            virtual ~Bundle();
            Bundle(const Bundle& other);

        std::string id, state, blueprint, spring,level, name;
        protected:

        private:
    };
}
#endif // BUNDLE_H
