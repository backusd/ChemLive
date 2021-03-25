#include "pch.h"
#include "DeviceResources.h"
#include "Main.h"
#include "Layout.h"
#include <ppltasks.h>	// For create_task

using winrt::Windows::ApplicationModel::SuspendingDeferral;
using winrt::Windows::ApplicationModel::SuspendingEventArgs;
using winrt::Windows::ApplicationModel::Activation::IActivatedEventArgs;
using winrt::Windows::ApplicationModel::Core::CoreApplication;
using winrt::Windows::ApplicationModel::Core::CoreApplicationView;
using winrt::Windows::ApplicationModel::Core::IFrameworkView;
using winrt::Windows::ApplicationModel::Core::IFrameworkViewSource;
using winrt::Windows::Graphics::Display::DisplayInformation;
using winrt::Windows::UI::Core::CoreCursor;
using winrt::Windows::UI::Core::CoreCursorType;
using winrt::Windows::UI::Core::CoreDispatcher;
using winrt::Windows::UI::Core::CoreDispatcherPriority;
using winrt::Windows::UI::Core::CoreProcessEventsOption;
using winrt::Windows::UI::Core::CoreWindow;
using winrt::Windows::UI::Core::CoreWindowEventArgs;
using winrt::Windows::UI::Core::PointerEventArgs;
using winrt::Windows::UI::Core::VisibilityChangedEventArgs;
using winrt::Windows::UI::Core::WindowActivatedEventArgs;
using winrt::Windows::UI::Core::WindowSizeChangedEventArgs;

using winrt::Windows::Foundation::IAsyncAction;
using winrt::Windows::UI::ViewManagement::ApplicationView;
using winrt::Windows::UI::ViewManagement::ApplicationViewSwitcher;

using winrt::Windows::UI::WindowManagement::AppWindow;

WINRT_EXPORT namespace ChemLive
{
    struct App : winrt::implements<App, IFrameworkViewSource, IFrameworkView>
    {
        IFrameworkView CreateView()
        {
            return *this;
        }

        // The first method called when the IFrameworkView is being created.
        void Initialize(CoreApplicationView const& applicationView)
        {
            // Register event handlers for app lifecycle. This example includes Activated, so that we
            // can make the CoreWindow active and start rendering on the window.
            applicationView.Activated({ this, &App::OnActivated });
            
            CoreApplication::Suspending({ this, &App::OnSuspending });
            CoreApplication::Resuming({ this, &App::OnResuming });

            // At this point we have access to the device. 
            // We can create the device-dependent resources.
            m_deviceResources = std::make_shared<DX::DeviceResources>();
        }

        // Initializes scene resources, or loads a previously saved app state.
        void Load(winrt::hstring const&)
        {
            if (m_main == nullptr)
            {
                m_main = std::unique_ptr<ChemLive::Main>(new ChemLive::Main(m_deviceResources));
            }
        }

        // Required for IFrameworkView.
        // Terminate events do not cause Uninitialize to be called. It will be called if your IFrameworkView
        // class is torn down while the app is in the foreground.
        void Uninitialize()
        {
        }

        // Called when the CoreWindow object is created (or re-created).
        void SetWindow(CoreWindow const& window)
        {
            window.PointerCursor(CoreCursor(CoreCursorType::Arrow, 0));

            window.Activated({ this, &App::OnWindowActivationChanged });
            window.SizeChanged({ this, &App::OnWindowSizeChanged });
            window.VisibilityChanged({ this, &App::OnVisibilityChanged });
            window.Closed({ this, &App::OnWindowClosed });            

            DisplayInformation displayInformation = DisplayInformation::GetForCurrentView();

            displayInformation.DpiChanged({ this, &App::OnDpiChanged });
            displayInformation.StereoEnabledChanged({ this, &App::OnStereoEnabledChanged });
            displayInformation.OrientationChanged({ this, &App::OnOrientationChanged });

            DisplayInformation::DisplayContentsInvalidated({ this, &App::OnDisplayContentsInvalidated });

            // Now that we have access to the CoreWindow, we can create the layout that
            // will define the pane layout for the app
            m_layout = std::shared_ptr<Layout>(new Layout(CoreWindow::GetForCurrentThread()));
            m_deviceResources->SetLayout(m_layout);          
        }

        void Run()
        {
            m_main->Run();
        }

        // =============================================================================
        // Application lifecycle event handlers.

        void OnActivated(CoreApplicationView, IActivatedEventArgs const&)
        {
            // Run() won't start until the CoreWindow is activated.
            CoreWindow::GetForCurrentThread().Activate();
        }
        void OnSuspending(IInspectable const&, SuspendingEventArgs args)
        {
            // Save app state asynchronously after requesting a deferral. Holding a deferral
            // indicates that the application is busy performing suspending operations. Be
            // aware that a deferral may not be held indefinitely. After about five seconds,
            // the app will be forced to exit.
            SuspendingDeferral deferral = args.SuspendingOperation().GetDeferral();

            concurrency::create_task([this, deferral]()
                {
                    m_deviceResources->Trim();

                    m_main->Suspend();

                    deferral.Complete();
                });
        }
        void OnResuming(IInspectable const& , IInspectable const&)
        {
            // Restore any data or state that was unloaded on suspend. By default, data
            // and state are persisted when resuming from suspend. Note that this event
            // does not occur if the app was previously terminated.  
            m_main->Resume();
        }

        // Window event handlers.

        void OnWindowActivationChanged(CoreWindow, WindowActivatedEventArgs e)
        {
            m_main->WindowActivationChanged(e.WindowActivationState());
        }
        void OnWindowSizeChanged(CoreWindow sender, WindowSizeChangedEventArgs)
        {
            if (m_layout->WindowSizeChanged())
            {
                m_deviceResources->WindowSizeChanged();
                m_main->CreateWindowSizeDependentResources();
            }
        }
        void OnVisibilityChanged(CoreWindow, VisibilityChangedEventArgs args)
        {
            m_main->Visibility(args.Visible());
            //m_windowVisible = args.Visible();
        }
        void OnWindowClosed(CoreWindow, CoreWindowEventArgs)
        {
            m_main->Close();
        }

        // DisplayInformation event handlers.

        void OnDpiChanged(DisplayInformation sender, IInspectable const&)
        {
            // Note: The value for LogicalDpi retrieved here may not match the effective DPI of the app
            // if it is being scaled for high resolution devices. Once the DPI is set on DeviceResources,
            // you should always retrieve it using the GetDpi method.
            // See DeviceResources.cpp for more details.

            if (m_layout->DPI(sender.LogicalDpi()))
            {
                m_deviceResources->SetDpi(sender.LogicalDpi());
                m_main->CreateWindowSizeDependentResources();
            }
        }
        void OnStereoEnabledChanged(DisplayInformation sender, IInspectable const&)
        {
            m_deviceResources->UpdateStereoState();
            m_main->CreateWindowSizeDependentResources();
        }
        void OnOrientationChanged(DisplayInformation sender, IInspectable const&)
        {
            m_deviceResources->SetCurrentOrientation(sender.CurrentOrientation());
            m_main->CreateWindowSizeDependentResources();
        }
        void OnDisplayContentsInvalidated(DisplayInformation, IInspectable const&)
        {
            m_deviceResources->ValidateDevice();
        }
        

        private:
            std::shared_ptr<DX::DeviceResources> m_deviceResources;
            std::unique_ptr<ChemLive::Main> m_main;
            std::shared_ptr<ChemLive::Layout> m_layout;
    };

    
}

int __stdcall wWinMain(HINSTANCE, HINSTANCE, PWSTR, int)
{
    CoreApplication::Run(winrt::make<ChemLive::App>());
}
