#ifndef SETTINGS_HPP
#define SETTINGS_HPP

class Settings {
    private:
        unsigned int screenWidth, screenHeight;
        float nearViewDistance, farViewDistance;
        float fov;
        unsigned int renderDistance; // in chunks
        bool wireframe;
    public:
        Settings();
        // getters and setters
        void setScreenWidth(unsigned int value);
        unsigned int getScreenWidth();
        void setScreenHeight(unsigned int value);
        unsigned int getScreenHeight();
        void setNearViewDistance(float value);
        float getNearViewDistance();
        void setFarViewDistance(float value);
        float getFarViewDistance();
        void setFov(float value);
        float getFov();
        void setRenderDistance(unsigned int value);
        unsigned int getRenderDistance();
        void setWireframe(bool value);
        bool getWireframe();
};
#endif