#pragma once

#include "StepTimer.h"

using winrt::Windows::UI::Core::CoreWindow;
using winrt::Windows::UI::Core::KeyEventArgs;
using winrt::Windows::UI::Core::PointerEventArgs;

using namespace DirectX;

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
        MoveLookController(CoreWindow window, XMFLOAT3 boxDimensions);

        bool MouseDown() const { return m_mouseDown; }
        float MousePositionX() const { return m_mousePositionXNew; }
        float MousePositionY() const { return m_mousePositionYNew; }

        bool IsMoving();

        XMMATRIX ViewMatrix();

        void Update(DX::StepTimer const& timer, D2D1_RECT_F renderPaneRect);


        void OnPointerPressed(CoreWindow, PointerEventArgs const&);
        void OnPointerMoved(CoreWindow, PointerEventArgs const&);
        void OnPointerReleased(CoreWindow, PointerEventArgs const&);

        void OnKeyDown(CoreWindow, KeyEventArgs const&);
        void OnKeyUp(CoreWindow, KeyEventArgs const&);

        XMVECTOR EyeVector() { return m_eyeVec; }

    private:
        void InitWindow(CoreWindow window);
        void ResetState();



        void RotateLeftRight(float theta);
        void RotateUpDown(float theta);


        // Pointer Variables
        bool m_mouseDown;

        float m_mousePositionX;
        float m_mousePositionY;
        float m_mousePositionXNew;
        float m_mousePositionYNew;

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
        bool                        m_shift;
        bool                        m_forward;
        bool                        m_back;
        bool                        m_left;
        bool                        m_right;
        bool                        m_up;
        bool                        m_down;
        bool                        m_pause;

        // Eye/at/up vectors
        XMVECTOR m_eyeVec;
        XMVECTOR m_atVec;
        XMVECTOR m_upVec;

        // Keep track of total time to be able to compute the time delta
        double m_elapsedTime;

        // Angle of rotation for computing the rotation matrix
        float m_xRadians;
        float m_yRadians;
        float m_zRadians;
    };
}