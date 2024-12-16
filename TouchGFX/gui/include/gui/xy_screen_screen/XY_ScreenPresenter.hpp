#ifndef XY_SCREENPRESENTER_HPP
#define XY_SCREENPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class XY_ScreenView;

class XY_ScreenPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    XY_ScreenPresenter(XY_ScreenView& v);

    /**
     * The activate function is called automatically when this screen is "switched in"
     * (ie. made active). Initialization logic can be placed here.
     */
    virtual void activate();

    /**
     * The deactivate function is called automatically when this screen is "switched out"
     * (ie. made inactive). Teardown functionality can be placed here.
     */
    virtual void deactivate();

    virtual ~XY_ScreenPresenter() {}

private:
    XY_ScreenPresenter();

    XY_ScreenView& view;
};

#endif // XY_SCREENPRESENTER_HPP
