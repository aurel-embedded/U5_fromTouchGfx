#ifndef MODEL_HPP
#define MODEL_HPP
#include <gui/model/ViewId.hpp>
#include <MDI_midi/CMidiCfg.h>
#include <MDI_midi/CMidiChannel.h>
#include <Tools/UserTypes.h>
#include <UserInterfaces/PotarManager/pmgr_thread.hpp>

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

    void setActiveView(ViewId view);
    ViewId getActiveView() const;


    void sendMidiOn(int note);
    void sendMidiOff(int note);
    void sendControlChange(CMidiChannel::Channel_e channel, uint8_t cc, uint8_t data);
    userTypes_6Uint16_t getAdcValuesMapped(uint16_t maxMappedVal);

    void setPmgrMode(pmgr_thread::FsmState state);

protected:
    ModelListener* modelListener;

private:
    ViewId activeViewId;
};

#endif // MODEL_HPP
