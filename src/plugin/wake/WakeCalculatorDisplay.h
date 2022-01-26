#pragma once
#include "euroscope/AsrEventHandlerInterface.h"
#include "radarscreen/MenuToggleableDisplayInterface.h"
#include "radarscreen/RadarRenderableInterface.h"

namespace UKControllerPlugin {
    namespace Components {
        class ClickableArea;
        class TitleBar;
    } // namespace Components
    namespace List {
        class PopupListInterface;
    } // namespace List
    namespace Euroscope {
        class EuroscopePluginLoopbackInterface;
    } // namespace Euroscope
} // namespace UKControllerPlugin

namespace UKControllerPlugin::Wake {
    class WakeCalculatorOptions;

    class WakeCalculatorDisplay : public RadarScreen::RadarRenderableInterface,
                                  public RadarScreen::MenuToggleableDisplayInterface,
                                  public Euroscope::AsrEventHandlerInterface
    {
        public:
        WakeCalculatorDisplay(
            std::shared_ptr<WakeCalculatorOptions> options,
            std::shared_ptr<List::PopupListInterface> leadCallsignSelector,
            std::shared_ptr<List::PopupListInterface> followCallsignSelector,
            std::shared_ptr<List::PopupListInterface> wakeSchemeSelector,
            Euroscope::EuroscopePluginLoopbackInterface& plugin,
            int screenObjectId);
        [[nodiscard]] auto IsVisible() const -> bool override;
        void LeftClick(
            Euroscope::EuroscopeRadarLoopbackInterface& radarScreen,
            int objectId,
            const std::string& objectDescription,
            POINT mousePos,
            RECT itemArea) override;
        void Move(RECT position, std::string objectDescription) override;
        void Render(
            Windows::GdiGraphicsInterface& graphics, Euroscope::EuroscopeRadarLoopbackInterface& radarScreen) override;
        void ResetPosition() override;
        void AsrLoadedEvent(Euroscope::UserSetting& userSetting) override;
        void AsrClosingEvent(Euroscope::UserSetting& userSetting) override;
        [[nodiscard]] auto Position() const -> const POINT&;
        auto MenuItem() const -> std::string override;
        void Toggle() override;

        private:
        [[nodiscard]] static auto TitleBarArea() -> Gdiplus::Rect;
        [[nodiscard]] static auto WindowContentArea() -> Gdiplus::Rect;
        void
        RenderScheme(Windows::GdiGraphicsInterface& graphics, Euroscope::EuroscopeRadarLoopbackInterface& radarScreen);
        void
        RenderLead(Windows::GdiGraphicsInterface& graphics, Euroscope::EuroscopeRadarLoopbackInterface& radarScreen);
        void RenderFollowing(
            Windows::GdiGraphicsInterface& graphics, Euroscope::EuroscopeRadarLoopbackInterface& radarScreen);
        void RenderIntermediate(
            Windows::GdiGraphicsInterface& graphics, Euroscope::EuroscopeRadarLoopbackInterface& radarScreen);
        void RenderDividingLine(Windows::GdiGraphicsInterface& graphics);
        void RenderSeparationRequirement(Windows::GdiGraphicsInterface& graphics);

        // The coordinate of the top left of the window
        inline static const POINT DEFAULT_WINDOW_POSITION{200, 200};
        POINT windowPosition = DEFAULT_WINDOW_POSITION;

        inline static const int WINDOW_WIDTH = 350;
        inline static const int TITLE_BAR_HEIGHT = 20;
        inline static const int CONTENT_HEIGHT = 125;
        inline static const int TEXT_INSET = 5;

        // Some options
        std::shared_ptr<WakeCalculatorOptions> options;

        // For selecting callsigns
        const std::shared_ptr<List::PopupListInterface> leadCallsignSelector;
        const std::shared_ptr<List::PopupListInterface> followCallsignSelector;

        // For selecting wake schemes
        const std::shared_ptr<List::PopupListInterface> wakeSchemeSelector;

        // For getting flightplans
        Euroscope::EuroscopePluginLoopbackInterface& plugin;

        // The screen object id for click
        const int screenObjectId;

        // The titlebar
        std::shared_ptr<Components::TitleBar> titleBar;

        // Pens and brushes
        const Gdiplus::Color BACKGROUND_COLOUR = Gdiplus::Color(64, 64, 64);
        const Gdiplus::Color TEXT_COLOUR = Gdiplus::Color(225, 225, 225);
        const Gdiplus::Color RESULT_COLOUR = Gdiplus::Color(55, 249, 1);
        std::shared_ptr<Gdiplus::Brush> backgroundBrush;
        std::shared_ptr<Gdiplus::Brush> textBrush;
        std::shared_ptr<Gdiplus::Brush> resultBrush;
        std::shared_ptr<Gdiplus::Pen> dividingLinePen;

        // Drawing rects
        Gdiplus::Rect contentArea;
        Gdiplus::Rect schemeStaticArea;
        Gdiplus::Rect schemeTextArea;
        Gdiplus::Rect intermediateStaticArea;
        Gdiplus::Rect intermediateTextArea;
        Gdiplus::Rect leadStaticArea;
        Gdiplus::Rect leadTextArea;
        Gdiplus::Rect followingStaticArea;
        Gdiplus::Rect followingTextArea;
        Gdiplus::Rect comparisonTextArea;
        Gdiplus::Point dividingLineStart;
        Gdiplus::Point dividingLineEnd;
        Gdiplus::Rect calculationResultArea;

        // Clickspot rects
        std::shared_ptr<Components::ClickableArea> leadClickspot;
        std::shared_ptr<Components::ClickableArea> followingClickspot;
        std::shared_ptr<Components::ClickableArea> schemeClickspot;
        std::shared_ptr<Components::ClickableArea> intermediateClickspot;

        // ASR Things
        const std::string ASR_KEY_VISIBILITY = "wakeCalculatorVisibility";
        const std::string ASR_DESCRIPTION_VISIBILITY = "Wake Calculator Visibility";
        const std::string ASR_KEY_X_POS = "wakeCalculatorXPosition";
        const std::string ASR_DESCRIPTION_X_POS = "Wake Calculator X Position";
        const std::string ASR_KEY_Y_POS = "wakeCalculatorYPosition";
        const std::string ASR_DESCRIPTION_Y_POS = "Wake Calculator Y Position";

        // Visibility
        bool visible = false;
    };
} // namespace UKControllerPlugin::Wake
