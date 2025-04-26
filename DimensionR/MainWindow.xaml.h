// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#pragma once

#include "MainWindow.g.h"

namespace winrt::DimensionR::implementation
{
    struct MainWindow : MainWindowT<MainWindow>
    {
        MainWindow();
        ~MainWindow();

        void init();

        int32_t MyProperty();
        void MyProperty(int32_t value);

        //void SpecialCase_Checked(IInspectable const&, RoutedEventArgs const&);
        //void SpecialCase_Unchecked(IInspectable const&, RoutedEventArgs const&);
        void SpecialCase_Checked(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args);
        void SpecialCase_Unchecked(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args);


        void CanvasControl_Draw(Microsoft::Graphics::Canvas::UI::Xaml::CanvasControl const& sender, Microsoft::Graphics::Canvas::UI::Xaml::CanvasDrawEventArgs const& args);
        void CanvasControl_PointerPressed(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::Input::PointerRoutedEventArgs const& args);
        void CanvasControl_PointerReleased(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::Input::PointerRoutedEventArgs const& args);
        void CanvasControl_PointerMoved(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::Input::PointerRoutedEventArgs const& args);

        void myButton_Click(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args);
        void myButton2_Click(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args);
        void myButton3_Click(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args);

        void R3_Click(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args);
        void R4_Click(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args);
        void R5_Click(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args);
        void R6_Click(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args);

        void NumberBox_NumberOfDimensions_ValueChanged(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::Controls::NumberBoxValueChangedEventArgs const& args);
        void NumberBox_NumberOfPoints_ValueChanged(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::Controls::NumberBoxValueChangedEventArgs const& args);

        void Slider_VisualDepth_ValueChanged(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs const& args);

        // TIMER STUFF
        void CreateTimerAndRegisterHandler();
        void UnregisterTimerHandler();
        void StartTimer();
        void StopTimer();
        void OnTick(IInspectable const& sender, IInspectable const& args);

        void ApplyAlgorithm();
        void ApplyIncrementAndNormalize(int index, double fraction);
        void CreateRandomUnitVector();
        void CreateRandomVector();

        std::wstring string_to_wstring(const std::string& text);
        void DEBUG_ShowValues();
        void DEBUG_ShowValues2();
        winrt::Windows::Foundation::Point ApplyTransform(double *p);

        int* GrowDuplicateTopology(int* initial, int numPoints, int numConnectionsPerPoint);
        void Reset();
        void CreateDynamicTopologyList();



    };
}

namespace winrt::DimensionR::factory_implementation
{
    struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow>
    {
    };
}
