#pragma once


using winrt::Windows::UI::Core::CoreWindow;
using winrt::Windows::UI::Core::PointerEventArgs;

namespace ChemLive
{
    enum class MoveLookControllerState
    {
        None,
        WaitForInput,
        Active,
    };

    namespace MoveLookConstants
    {
        static const float RotationGain = 0.008f;       // Sensitivity adjustment for look controller.
        static const float MovementGain = 2.0f;         // Sensitivity adjustment for move controller.
    };

	class MoveLookController
	{
    public:
        MoveLookController(CoreWindow window);

        bool MouseDown() const { return m_mouseDown; }
        float MousePositionX() const { return m_mousePositionX; }

    private:
        void InitWindow(CoreWindow window);
        void ResetState();


        void OnPointerPressed(CoreWindow, PointerEventArgs const&);
        void OnPointerMoved(CoreWindow, PointerEventArgs const&);
        void OnPointerReleased(CoreWindow, PointerEventArgs const&);


        // Pointer Variables
        bool m_mouseDown;
        float m_mousePositionX;

        // Properties of the controller object.
        MoveLookControllerState     m_state;
        float                       m_pitch;
        DirectX::XMFLOAT3           m_velocity;             // How far we move in this frame.
        float                       m_yaw;                  // Orientation euler angles in radians.
	
        // Properties of the Move control.
        DirectX::XMFLOAT2           m_moveUpperLeft;        // Bounding box where this control will activate.
        DirectX::XMFLOAT2           m_moveLowerRight;
        bool                        m_moveInUse;            // The move control is in use.
        uint32_t                    m_movePointerID;        // Id of the pointer in this control.
        DirectX::XMFLOAT2           m_moveFirstDown;        // Point where initial contact occurred.
        DirectX::XMFLOAT2           m_movePointerPosition;  // Point where the move pointer is currently located.
        DirectX::XMFLOAT3           m_moveCommand;          // The net command from the move control.

        // Properties of the Look control.
        bool                        m_lookInUse;            // The look control is in use.
        uint32_t                    m_lookPointerID;        // Id of the pointer in this control.
        DirectX::XMFLOAT2           m_lookLastPoint;        // Last point (from last frame).
        DirectX::XMFLOAT2           m_lookLastDelta;        // The delta used for smoothing between frames.

        // Properties of the Mouse control.  This is a combination of Look and Fire.
        bool                        m_mouseInUse;
        uint32_t                    m_mousePointerID;
        DirectX::XMFLOAT2           m_mouseLastPoint;
        bool                        m_mouseLeftInUse;
        bool                        m_mouseRightInUse;

        bool                        m_buttonInUse;
        uint32_t                    m_buttonPointerID;
        DirectX::XMFLOAT2           m_buttonUpperLeft;
        DirectX::XMFLOAT2           m_buttonLowerRight;
        bool                        m_buttonPressed;
        bool                        m_pausePressed;

        // Input states for Keyboard.
        bool                        m_forward;
        bool                        m_back;
        bool                        m_left;
        bool                        m_right;
        bool                        m_up;
        bool                        m_down;
        bool                        m_pause;
    };
}