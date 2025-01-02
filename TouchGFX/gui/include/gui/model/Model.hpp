#ifndef MODEL_HPP
#define MODEL_HPP
#include <MDI_midi/CMidiCfg.h>
#include <MDI_midi/CMidiChannel.h>

class ModelListener;

class Model
{
public:
    Model();

    void bind(ModelListener* listener)
    {
        modelListener = listener;
    }

    void tick();

    void sendMidiOn(int note);
    void sendMidiOff(int note);
    void sendControlChange(CMidiChannel::Channel_e channel, CMidiCfg::cc cc, uint8_t data);

protected:
    ModelListener* modelListener;
};

#endif // MODEL_HPP
