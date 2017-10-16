#ifndef BUNDLE_H
#define BUNDLE_H

#include <string>
#include <chrono>
#include <ctime>
#include <ratio>


namespace _ESBManager
{
    class Bundle
    {
        public:
            Bundle();
            virtual ~Bundle();
            Bundle(const Bundle& other);

            void SetAccumulatedTimeBeforFailure(std::chrono::duration<int64_t, std::milli> accumulatedTimeBeforFailure) {
                this->accumulatedTimeBeforFailure = accumulatedTimeBeforFailure;
            }

            std::chrono::duration<int64_t, std::milli> GetAccumulatedTimeBeforFailure() const {
                return accumulatedTimeBeforFailure;
            }

            void SetAccummulatedTimeBeforeRepair(std::chrono::duration<int64_t, std::milli> accummulatedTimeBeforeRepair) {
                this->accummulatedTimeBeforeRepair = accummulatedTimeBeforeRepair;
            }

            std::chrono::duration<int64_t, std::milli> GetAccummulatedTimeBeforeRepair() const {
                return accummulatedTimeBeforeRepair;
            }

            void SetLastRestartTime(std::chrono::time_point<std::chrono::system_clock> lastRestartTime) {
                this->lastRestartTime = lastRestartTime;
            }

            std::chrono::time_point<std::chrono::system_clock> GetLastRestartTime() const {
                return lastRestartTime;
            }

            void IncrementRestartCount() {
                this->restartCount++;
            }

            int GetRestartCount() const {
                return restartCount;
            }

            void SetName(std::string name) {
                this->name = name;
            }

            std::string GetName() const {
                return name;
            }

            void SetLevel(std::string level) {
                this->level = level;
            }

            std::string GetLevel() const {
                return level;
            }

            void SetSpring(std::string spring) {
                this->spring = spring;
            }

            std::string GetSpring() const {
                return spring;
            }

            void SetBlueprint(std::string blueprint) {
                this->blueprint = blueprint;
            }

            std::string GetBlueprint() const {
                return blueprint;
            }

            void SetState(std::string state) {
                this->state = state;
            }

            std::string GetState() const {
                return state;
            }

            void SetId(std::string id) {
                this->id = id;
            }

            std::string GetId() const {
                return id;
            }

        std::string id, state, blueprint, spring,level, name;
        protected:

        private:
            int restartCount;
            std::chrono::time_point<std::chrono::system_clock> lastRestartTime;
            std::chrono::duration<int64_t, std::milli>  accummulatedTimeBeforeRepair, 
                                                        accumulatedTimeBeforFailure;
    };
}
#endif // BUNDLE_H
