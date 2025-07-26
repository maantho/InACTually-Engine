//
// Created by Jonas Höpner on 10.07.23.
//

#pragma once

namespace act::proc {
/**
 * helper class for marker positions in MarkerPositionDetection.
 * Contains all needed properties of the MarkerSceneNode.
 *
 * We need this workaround, as we cannot create MarkerSceneNodes if not in main thread. The MarkerSceneNode is created with textures and such,
 *  and these require the current OpenGL Context, which does not exist in a thread.
 */
class MarkerPosition {
protected:
    glm::vec3 position;
    glm::vec3 rotation;
    int id;
    bool fixed;

public:

    MarkerPosition(int id, glm::vec3 position, glm::vec3 rotation) : id(id), position(position), rotation(rotation) {
        this->fixed = false;
    }
    MarkerPosition(int id, glm::vec3 position) : id(id), position(position) {
        this->fixed = false;
    }
    MarkerPosition(int id) : id(id) {
        this->fixed = false;
    }

    int getId() const {
        return id;
    }

    void setId(int id) {
        this->id = id;
    }

    bool isFixed() const {
        return fixed;
    }

    bool isDynamic() {
        return !(this->fixed);
    }

    void setIsFixed(bool fixed) {
        this->fixed = fixed;
    }

    glm::vec3 getPosition() const {
        return position;
    }

    void setPosition(glm::vec3 position) {
        if (this->fixed) return;
        this->position = position;
    }

    glm::vec3 getRotation() const {
        return rotation;
    }

    void setRotation(glm::vec3 rotation) {
        if (this->fixed) return;
        this->rotation = rotation;
    }

};

}
