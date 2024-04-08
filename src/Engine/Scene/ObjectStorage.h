#pragma once

#include <typeindex>
#include "../utils/SparseSet.h"
#include "../BaseObject/BaseObject.h"
#include "../utils/Grid.h"
#include <bits/shared_ptr.h>
#include <set>
#include <unordered_map>

class ObjectStorage {
private:
    int m_maxObjectsNum;
    Scene *m_scene;
    std::vector<std::shared_ptr<BaseObject>> m_objects;
    std::set<int, std::greater<>> m_objectsToRemove;
    SparseSet m_objectsWithRotation;
    std::vector<BasicDetails> m_basicDetails;
    std::unordered_map<std::type_index, std::set<int>> m_objectTypesIndexLists;
    int m_callbackIdCounter = 0;
    std::unordered_map<int, std::function<void(int, int)>> m_elementTransferCallbacks;

    void markObjectForRemoval(int index) {
        m_objectsToRemove.insert(index);
    }

    int getIndexByPtr(BaseObject *ptr) {
        auto it = m_objects.end();
        for (auto i = m_objects.begin(); i != m_objects.end(); i++) {
            if (i->get() == ptr) {
                it = i;
                break;
            }
        }
        if (it == m_objects.end()) {
            throw std::runtime_error("Trying to get index of ptr to object that does not exist");
        }
        return static_cast<int>(it - m_objects.begin());
    }

    int getIndexByPtr(std::weak_ptr<BaseObject> &ptr) {
        auto it = std::find(m_objects.begin(), m_objects.end(), ptr.lock());
        if (it == m_objects.end()) {
            throw std::runtime_error("Trying to get index of ptr to object that does not exist");
        }
        return static_cast<int>(it - m_objects.begin());
    }

public:

    explicit ObjectStorage(int maxObjectsNum, Scene* scene) :
            m_maxObjectsNum(maxObjectsNum),
            m_scene(scene),
            m_objectsWithRotation(maxObjectsNum) {
        m_objects.reserve(maxObjectsNum);
        m_basicDetails.reserve(maxObjectsNum);
    }

    template<typename T>
    int addElementTransferCallback(T callback) {
        m_elementTransferCallbacks[m_callbackIdCounter] = callback;
        return m_callbackIdCounter++;
    }

    void removeElementTransferCallback(int id) {
        m_elementTransferCallbacks.erase(id);
    }

    void toggleObjectRotation(BaseObject *ptr, bool enabled) {
        int index = getIndexByPtr(ptr);
        if (enabled) {
            m_objectsWithRotation.insert(index);
        } else {
            m_objectsWithRotation.remove(index);
        }
    }

    void toggleObjectRotation(int index, bool enabled) {
        if (enabled) {
            m_objectsWithRotation.insert(index);
        } else {
            m_objectsWithRotation.remove(index);
        }
    }

    template<typename T>
    std::weak_ptr<BaseObject> addObject(T &&object, Vector2F position) {
        if (m_objects.size() + 1 > m_maxObjectsNum) {
            throw std::runtime_error("Exceeded Scene capacity");
        }

        m_basicDetails.emplace_back(position);
        object.m_basicDetails = &m_basicDetails.back();
        object.m_scene = m_scene;

        m_objects.push_back(std::make_shared<T>(std::forward<T>(object)));

        int index = static_cast<int>(m_objects.size()) - 1;
        m_basicDetails[index].m_parent = m_objects[index].get();
        m_objects[index]->m_id = index;
        m_objects[index]->v_onInit();
        m_objectTypesIndexLists[typeid(T)].insert(index);
        return {m_objects[index]};
    }

    [[nodiscard]] int getObjectsCount() {
        return static_cast<int>(m_objects.size());
    }

    void removeMarkedObjects() {
        // use std::set to have objectsToRemove in descending order
        // Because there's bad corner case:
        // if hypotetically vector is of size 100, and "removal set" is {15, 20, 99, 98},
        // then we would remove 15, swapped it with 99, popped, and element 99 would be at position 15. And removal at position 99 would fail
        if (m_objectsToRemove.empty()) {
            return;
        }

        // use swap & pop to remove objects without a lot of shifting
        for (const auto &i: m_objectsToRemove) {
            int lastIndex = getObjectsCount() - 1;
            if (i > lastIndex || i < 0) {
                throw std::runtime_error("Trying to remove object that does not exist");
            }

            if (i == lastIndex) {
                for (const auto &callback: m_elementTransferCallbacks) {
                    callback.second(i, -1);
                }

                // this actually works. typeid(removedObjectRef) returns the type of the derived object, not BaseObject
                // be cautious, that if I would use typeid directly on pointer, it would return BaseObject for any derived object
                BaseObject &removedObjectRef = *(m_objects[i].get());
                m_objectTypesIndexLists[typeid(removedObjectRef)].erase(i);

                m_objectsWithRotation.remove(i);
                m_objects.pop_back();
                m_basicDetails.pop_back();
            }
            if (i < lastIndex) {
                BaseObject &removedObjectRef = *m_objects[i];
                BaseObject &lastObjectRef = *m_objects[lastIndex];
                std::type_index removedTypeIndex = typeid(removedObjectRef);
                std::type_index lastTypeIndex = typeid(lastObjectRef);
                m_objectTypesIndexLists[removedTypeIndex].erase(i);
                m_objectTypesIndexLists[lastTypeIndex].erase(lastIndex);
                m_objectTypesIndexLists[lastTypeIndex].insert(i);

                if (m_objectsWithRotation.contains(lastIndex)) {
                    m_objectsWithRotation.insert(i);
                    m_objectsWithRotation.remove(lastIndex);
                } else {
                    m_objectsWithRotation.remove(i);
                }
                std::swap(m_objects[i], m_objects[lastIndex]);
                std::swap(m_basicDetails[i], m_basicDetails[lastIndex]);
                m_objects.pop_back();
                m_basicDetails.pop_back();
                m_objects[i]->m_basicDetails = &m_basicDetails[i];
                m_objects[i]->m_id = i;

                for (const auto &callback: m_elementTransferCallbacks) {
                    callback.second(lastIndex, i);
                    callback.second(i, -1);
                }
            }
        }

        m_objectsToRemove.clear();
    }

    void removeObject(int index) {
        // Important node: removal itself happens in the end of the frame. After all physics substeps and input handling.
        // So, for example, checking weakPointer.expired() in the same frame after removeObject() will return false
        markObjectForRemoval(index);
    }

    void removeObject(std::weak_ptr<BaseObject> &ptr) {
        int index = getIndexByPtr(ptr);
        markObjectForRemoval(index);
    }

    void removeObject(BaseObject *ptr) {
        // theres was bug: if you remove an object during m_physics calculations, it will cause some m_grid m_objects to become invalid (because everything is shifted), and m_grid uses id's
        // this can happen primarily if we for example remove an object in collision m_callback
        // to fix this we  use a stack of m_objects to remove, and remove them after m_physics calculations (and input handling)
        int index = getIndexByPtr(ptr);
        markObjectForRemoval(index);
    }

    template<typename T>
    void forEachObject(const T &callback, int start = 0, int end = -1) {
        // as i've discovered.. std::function HAS overhead. It was taking around 2 fps. So, i use T here. for performance-critical code parts better to not use std::function
        if (end == -1) {
            end = static_cast<int>(m_objects.size());
        }

        for (int i = start; i < end; i++) {
            callback(*m_objects[i], i);
        }
    }

    template<typename ObjectType, typename T>
    void forEachObjectOfType(const T &callback) {
        // function is pretty fast. It uses static_cast for casting.
        // And internal list of indices for each type of object
        // But can be improved by using std::vector instead of std::set,
        // and storing pointers instead of indices to avoid indirection
        for (int i: m_objectTypesIndexLists[typeid(ObjectType)]) {
            callback(static_cast<ObjectType &>(*m_objects[i]), i);
        }
    }

    template<typename ObjectType>
    int getObjectsOfTypeCount() {
        // this fast, as it doesn't actually count anything in loop, but just get's size of set
        return static_cast<int>(m_objectTypesIndexLists[typeid(ObjectType)].size());
    }

    template<typename T>
    void forEachBasicDetails(const T &callback, int start = 0, int end = -1) {
        if (end == -1) {
            end = static_cast<int>(m_basicDetails.size());
        }

        for (int i = start; i < end; i++) {
            callback(m_basicDetails[i], i);
        }
    }

    template<typename T>
    void forEachBasicDetailsWithRotation(const T &callback, int start = 0, int end = -1) {
        m_objectsWithRotation.forEach([&](int id, int iteration) {
            callback(m_basicDetails[id], id, iteration);
        }, start, end);
    }

    BaseObject &getObject(int ind) {
        return *m_objects[ind];
    }

    BasicDetails &getBasicDetails(int ind) {
        return m_basicDetails[ind];
    }

    [[nodiscard]] int getObjectsWithRotationCount() const {
        return m_objectsWithRotation.size();
    }

    void logDifferentTypeCounts() {
        for (const auto &i: m_objectTypesIndexLists) {
            std::cout << i.first.name() << " " << i.second.size() << std::endl;
        }
    }

    int getMaxObjectsCount() const {
        return m_maxObjectsNum;
    }
};