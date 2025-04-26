// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#include "pch.h"
#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

using namespace winrt::Microsoft::UI::Xaml::Media;
using namespace winrt::Microsoft::UI::Xaml::Input;
using namespace winrt::Microsoft::UI::Input;
using namespace winrt::Microsoft::UI::Xaml::Controls;

using namespace winrt::Windows::Foundation;

using namespace winrt::Microsoft::UI;
using namespace winrt::Microsoft::Graphics::Canvas::UI::Xaml;
using namespace winrt::Microsoft::Graphics::Canvas::Brushes;
using namespace winrt::Windows::UI::ViewManagement;

using namespace winrt::Microsoft::Graphics::Canvas::Text;
using namespace winrt::Microsoft::Graphics::Canvas;

using namespace std;
using namespace arma;

using namespace winrt::Windows::ApplicationModel::Background;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::DimensionR::implementation
{
    struct SampleBackgroundTask : implements<SampleBackgroundTask, IBackgroundTask>
    {
        void Run(IBackgroundTaskInstance const& taskInstance)
        {
            // TODO: Insert code to perform background work
            for (int i = 0; i < 10000; i++)
            {
                //ApplyAlgorithm();
            }

        }
    };

    BackgroundTaskBuilder gBuilder;

    struct sort_item
    {
        int index;
        double val;
    };

    bool sort_function(const sort_item& lhs, const sort_item& rhs)
    {
        return lhs.val < rhs.val;
    }

    // GLOBALS
    Microsoft::UI::Xaml::DispatcherTimer gTimer{ nullptr };
    winrt::event_token gTimerHandlerRegistrationToken;

    double gX = 0;
    double gY = 0;
    double gX0 = 0;
    double gY0 = 0;
    double gdX = 0;
    double gdY = 0;
    bool gMouseDownLeft = false;
    bool gMouseMoved = false;

    int gTickCount = 0;


 //   int gNumPoints = 64;
//    int gNumDimensions = 6;
//    int gNumPoints = 32;
//    int gNumDimensions = 5;
    bool gFollowTopology = false;
    int gNumPoints = 8;
    int gNumDimensions = 3;
    double* gPoints = nullptr;
    double* gPointsT = nullptr;

    double gDepthScale = 1.0;

    sort_item* gSortList = nullptr;

    //double* grvec = nullptr;
    double grvec[100];

    int* gTopology = nullptr;
    int* gTopology2 = nullptr;
    int* gTopology3 = nullptr;
    int* gTopology4 = nullptr;
    int* gTopology5 = nullptr;
    int* gTopology6 = nullptr;

    int* gDynamicTopology = nullptr;


    double gTheta = 0;
    double gThetaInc = 0;

    int gTopology1[] = {1,0};

    int gTopology3OLD[] =  {1,3,4,
                        0,2,5,
                        1,3,6,
                        0,2,7,
                        0,5,7,
                        1,4,6,
                        2,5,7,
                        3,4,6
    };

    int gTopology4OLD[] = { 1,3,4,8,
                        0,2,5,9,
                        1,3,6,10,
                        0,2,7,11,
                        0,5,7,12,
                        1,4,6,13,
                        2,5,7,14,
                        3,4,6,15,

                        0,9,11,12,
                        1,8,10,13,
                        2,9,11,14,
                        3,8,10,15,
                        4,8,13,15,
                        5,9,12,14,
                        6,10,13,15,
                        7,11,12,14,

                        0,17,19,20,
                        1,16,18,21,
                        2,17,19,22,
                        3,16,18,23,
                        4,16,21,23,
                        5,17,20,22,
                        6,18,21,23,
                        7,19,20,22
    };



    MainWindow::MainWindow()
    {
        InitializeComponent();
    }

    MainWindow::~MainWindow()
    {
        delete gPoints;
        delete gPointsT;
        delete gTopology2;
        delete gTopology3;
        delete gTopology4;
        delete gTopology5;
        delete gTopology6;
        delete gSortList;
        delete gDynamicTopology;
    }

    void MainWindow::init()
    {
        // Initialize random seed
        srand(time(NULL));

        // create the array of points and zero them out
        gPoints = new double[gNumPoints * gNumDimensions];
        gPointsT = new double[gNumPoints * gNumDimensions];

        gSortList = new sort_item[gNumPoints];
        gDynamicTopology = new int[gNumPoints * gNumDimensions * 2];

        for (int i = 0; i < 100; i++) { grvec[i] = 0.0; }

        for (int i = 0; i < gNumPoints; i++)
        {

            //CreateRandomUnitVector();
            CreateRandomVector();

            for (int j = 0; j < gNumDimensions; j++)
            {
                gPoints[i * gNumDimensions + j] = grvec[j];
                gPointsT[i * gNumDimensions + j] = 0.0;
            }
            //delete temp;
        }

        //const int n = 8;

        // TEST
        gTopology2 = nullptr;
        gTopology2 = GrowDuplicateTopology(gTopology1, 2, 1);
        gTopology3 = nullptr;
        gTopology3 = GrowDuplicateTopology(gTopology2, 4, 2);
        gTopology4 = nullptr;
        gTopology4 = GrowDuplicateTopology(gTopology3, 8, 3);
        gTopology5 = nullptr;
        gTopology5 = GrowDuplicateTopology(gTopology4, 16, 4);
        gTopology6 = nullptr;
        gTopology6 = GrowDuplicateTopology(gTopology5, 32, 5);

        gTopology = gTopology6;
        if (gNumDimensions == 3) { gTopology = gTopology3; }
        else if (gNumDimensions == 4) { gTopology = gTopology4; }
        else if (gNumDimensions == 5) { gTopology = gTopology5; }
        else if (gNumDimensions == 6) { gTopology = gTopology6; }



        //gBuilder.Name(L"SampleBackgroundTask");
        //gBuilder.TaskEntryPoint(L"SampleBackgroundTask.SampleBackgroundTask");
        //gBuilder.SetTrigger(SystemTrigger(SystemTriggerType::TimeZoneChange, false));
        //BackgroundTaskRegistration registration = gBuilder.Register();


        // create a render timer
        CreateTimerAndRegisterHandler();
        StartTimer();
    }

    int* MainWindow::GrowDuplicateTopology(int* initial, int numPoints, int numConnectionsPerPoint)
    {
        int* topology = new int[2 * numPoints * (numConnectionsPerPoint + 1)];

        for (int i = 0; i < numPoints; i++)
        {
            for (int j = 0; j < numConnectionsPerPoint + 1; j++) {
                if(j < numConnectionsPerPoint )
                {
                    int val = initial[i * numConnectionsPerPoint + j];

                    topology[i * (numConnectionsPerPoint + 1) + j] = val;
                    topology[numPoints * (numConnectionsPerPoint + 1) + i * (numConnectionsPerPoint + 1) + j] = val + numPoints;
                }
                else
                {
                    topology[i * (numConnectionsPerPoint + 1) + j] = i + numPoints;
                    topology[numPoints * (numConnectionsPerPoint + 1) + i * (numConnectionsPerPoint + 1) + j] = i;
                }
            }
        }
        return topology;
    }

    void MainWindow::CreateDynamicTopologyList()
    {
        for (int i = 0; i < gNumPoints; i++)
        {
            double* pt1 = &gPoints[i * gNumDimensions];

            // create the sort list for a given point
            for (int j = 0; j < gNumPoints; j++)
            {
                double* pt2 = &gPoints[j * gNumDimensions];

                double length = 0;
                for (int k = 0; k < gNumDimensions; k++)
                {
                    length += pow(pt1[k] - pt2[k], 2);
                }
                length = sqrt(length);

                gSortList[j].index = j;
                gSortList[j].val = length;
            }

            // sort the list
            std::sort(gSortList, gSortList + gNumPoints, &sort_function);

            // enter the indexes of the closest points into the dynamic topology list, but
            // ignore the first point because it is the comparison with itself
// ******
            for(int k = 0; k < gNumDimensions * 2; k++)
            {
                //gDynamicTopology[i * gNumDimensions + k] = gSortList[k + 1].index;
                gDynamicTopology[i * gNumDimensions * 2 + k] = gSortList[k + 1].index;
            }
        }

        gTopology = gDynamicTopology;
    }

    int32_t MainWindow::MyProperty()
    {
        throw hresult_not_implemented();
    }

    void MainWindow::MyProperty(int32_t /* value */)
    {
        throw hresult_not_implemented();
    }


    void  MainWindow::SpecialCase_Checked(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args)
    {
        gNumPoints = 8;
        gNumDimensions = 3;
        gTopology = gTopology3;
        NumberBox_NumberOfDimensions().Value((double)gNumDimensions);
        NumberBox_NumberOfPoints().Value((double)gNumPoints);

        NumberBox_NumberOfDimensions().IsEnabled(false);
        NumberBox_NumberOfPoints().IsEnabled(false);

        gFollowTopology = true;

        Reset();

    }

    void  MainWindow::SpecialCase_Unchecked(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args)
    {
        gFollowTopology = false;

        NumberBox_NumberOfDimensions().IsEnabled(true);
        NumberBox_NumberOfPoints().IsEnabled(true);

    }

    void MainWindow::R3_Click(IInspectable const&, RoutedEventArgs const&)
    {
        gNumPoints = 8;
        gNumDimensions = 3;
        gTopology = gTopology3;
        NumberBox_NumberOfDimensions().Value((double)gNumDimensions);
        NumberBox_NumberOfPoints().Value((double)gNumPoints);

        Reset();
    }

    void MainWindow::R4_Click(IInspectable const&, RoutedEventArgs const&)
    {
        gNumPoints = 16;
        gNumDimensions = 4;
        gTopology = gTopology4;
        NumberBox_NumberOfDimensions().Value((double)gNumDimensions);
        NumberBox_NumberOfPoints().Value((double)gNumPoints);
        Reset();
    }

    void MainWindow::R5_Click(IInspectable const&, RoutedEventArgs const&)
    {
        gNumPoints = 32;
        gNumDimensions = 5;
        gTopology = gTopology5;
        NumberBox_NumberOfDimensions().Value((double)gNumDimensions);
        NumberBox_NumberOfPoints().Value((double)gNumPoints);
        Reset();
    }

    void MainWindow::R6_Click(IInspectable const&, RoutedEventArgs const&)
    {
        gNumPoints = 64;
        gNumDimensions = 6;
        gTopology = gTopology6;
        NumberBox_NumberOfDimensions().Value((double)gNumDimensions);
        NumberBox_NumberOfPoints().Value((double)gNumPoints);
        Reset();
    }

    void MainWindow::myButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        Reset();
    }

    void MainWindow::Reset()
    {
        delete gPoints;
        delete gPointsT;
        delete gSortList;
        delete gDynamicTopology;

        // create the array of points and zero them out
        gPoints = new double[gNumPoints * gNumDimensions];
        gPointsT = new double[gNumPoints * gNumDimensions];

        gSortList = new sort_item[gNumPoints];
        gDynamicTopology = new int[gNumPoints * gNumDimensions * 2];
//        gTopology = gDynamicTopology;


        for (int i = 0; i < gNumPoints; i++)
        {
            CreateRandomUnitVector();

            gSortList[i].index = 0;
            gSortList[i].val = 0;

            for (int j = 0; j < gNumDimensions; j++)
            {
                gPoints[i * gNumDimensions + j] = grvec[j];
                gPointsT[i * gNumDimensions + j] = 0.0;
            }
        }

        //CreateDynamicTopologyList();

        // DEBUG ***************
        //DEBUG_ShowValues2();

        gTickCount = 0;

    }

    void MainWindow::myButton2_Click(IInspectable const&, RoutedEventArgs const&)
    {
        //myButton2().Content(box_value(L"Clicked"));
        DEBUG_ShowValues();
        //DEBUG_ShowValues2();
    }

    void MainWindow::myButton3_Click(IInspectable const&, RoutedEventArgs const&)
    {
        //myButton2().Content(box_value(L"Clicked"));
        gThetaInc = 2.0 * 3.14 / 180.0;
        gTheta = 0;
    }


    winrt::Windows::Foundation::Point MainWindow::ApplyTransform(double *p)
    {
        double x = p[0];
        double y = p[1];

        if (gNumDimensions == 3)
        {
            x *= sqrt(3);
            y *= sqrt(3);
        }
        else if (gNumDimensions == 4)
        {
            //x *= 1.0 - (0.5 - p[3]) * 0.4;
            //y *= 1.0 - (0.5 - p[3]) * 0.4;
//            x *= (sqrt(4) / (3.0)) * (2.0 + p[3] * sqrt(4));
//            y *= (sqrt(4) / (3.0)) * (2.0 + p[3] * sqrt(4));
            x *= gDepthScale * ((sqrt(4) / 3.0) * (2.0 + p[3] * sqrt(4)) - sqrt(4)) + sqrt(4);
            y *= gDepthScale * ((sqrt(4) / 3.0) * (2.0 + p[3] * sqrt(4)) - sqrt(4)) + sqrt(4);
        }
        else if (gNumDimensions == 5)
        {
            //x *= 1.0 - (2.0 / sqrt(5) - p[3] - p[4]) * 0.4;
            //y *= 1.0 - (2.0 / sqrt(5) - p[3] - p[4]) * 0.4;
 //         x *= (sqrt(5) / (3.0 * 5.0)) * (2.0 + p[3] * sqrt(5)) * (4.0 + p[4] * sqrt(5));
 //         y *= (sqrt(5) / (3.0 * 5.0)) * (2.0 + p[3] * sqrt(5)) * (4.0 + p[4] * sqrt(5));
            x *= gDepthScale * ((sqrt(5) / (3.0 * 5.0)) * (2.0 + p[3] * sqrt(5)) * (4.0 + p[4] * sqrt(5)) - sqrt(5)) + sqrt(5);
            y *= gDepthScale * ((sqrt(5) / (3.0 * 5.0)) * (2.0 + p[3] * sqrt(5)) * (4.0 + p[4] * sqrt(5)) - sqrt(5)) + sqrt(5);
        }
        else if (gNumDimensions == 6)
        {
            //x *=  1.0 - (3.0 / sqrt(6) - p[3] - p[4] - p[5]) * 0.16;
            //y *=  1.0 - (3.0 / sqrt(6) - p[3] - p[4] - p[6]) * 0.16;
 //           x *= (sqrt(6) / (3.0 * 5.0 * 9.0)) * (2.0 + p[3] * sqrt(6)) * (4.0 + p[4] * sqrt(6)) * (8.0 + p[5] * sqrt(6));
 //           y *= (sqrt(6) / (3.0 * 5.0 * 9.0)) * (2.0 + p[3] * sqrt(6)) * (4.0 + p[4] * sqrt(6)) * (8.0 + p[5] * sqrt(6));
            x *= gDepthScale * ((sqrt(6) / (3.0 * 5.0 * 9.0)) * (2.0 + p[3] * sqrt(6)) * (4.0 + p[4] * sqrt(6)) * (8.0 + p[5] * sqrt(6)) - sqrt(6)) + sqrt(6);
            y *= gDepthScale * ((sqrt(6) / (3.0 * 5.0 * 9.0)) * (2.0 + p[3] * sqrt(6)) * (4.0 + p[4] * sqrt(6)) * (8.0 + p[5] * sqrt(6)) - sqrt(6)) + sqrt(6);
        }

        x = 400.0 + 125.0 * x;
        y = 200.0 + 125.0 * y;

        return Point(x,y);
    }


    void MainWindow::CanvasControl_Draw(CanvasControl const& sender, CanvasDrawEventArgs const& args)
    {

        winrt::Windows::UI::Color black = Windows::UI::ColorHelper::FromArgb(255, 0, 15, 30);
        args.DrawingSession().Clear(black);


        winrt::Windows::UI::Color colour1 = Windows::UI::ColorHelper::FromArgb(255, 40, 250, 40);
        winrt::Windows::UI::Color colour2 = Windows::UI::ColorHelper::FromArgb(50, 250, 250, 250);
        //winrt::Windows::UI::Color colour2 = Windows::UI::ColorHelper::FromArgb(0.20, 0.95, 0.95, 0.95);
        winrt::Windows::UI::Color colour3 = Windows::UI::ColorHelper::FromArgb(255, 250, 125, 0);
        winrt::Windows::UI::Color colour4 = Windows::UI::ColorHelper::FromArgb(255, 255, 250, 0);
        winrt::Windows::UI::Color blueprint = Windows::UI::ColorHelper::FromArgb(70, 0, 125, 255);

        if(gFollowTopology)
        {
            if (gNumDimensions >= 3)
            {
                // DRAW LINES - white ones first
                for (int i = 0; i < gNumPoints; i++)
                {
                    //for (int j = 0; j < gNumDimensions * 2; j++)
                    for (int j = 0; j < gNumDimensions; j++)
                    {
                        int index = gTopology[i * gNumDimensions + j];
                        //int index = gTopology[i * gNumDimensions * 2 + j];

                        Point p0 = ApplyTransform(&gPointsT[i * gNumDimensions]);
                        Point p1 = ApplyTransform(&gPointsT[index * gNumDimensions]);

                        if ((i >= 0 && i <= 7) && (index >= 0 && index <= 7))
                        {
                            //args.DrawingSession().DrawLine(p0, p1, colour1, 2.5f);
                        }
                        else { args.DrawingSession().DrawLine(p0, p1, colour2, 5.5f); }
                    }
                }

                // DRAW LINES - coloured ones last
                for (int i = 0; i < gNumPoints; i++)
                {
                    //for (int j = 0; j < gNumDimensions * 2; j++)
                    for (int j = 0; j < gNumDimensions; j++)
                    {
                        int index = gTopology[i * gNumDimensions + j];
                        //int index = gTopology[i * gNumDimensions * 2 + j];

                        Point p0 = ApplyTransform(&gPointsT[i * gNumDimensions]);
                        Point p1 = ApplyTransform(&gPointsT[index * gNumDimensions]);

                        if ((i >= 0 && i <= 7) && (index >= 0 && index <= 7))
                        {
                            args.DrawingSession().DrawLine(p0, p1, colour1, 5.5f);
                        }
                        //else { args.DrawingSession().DrawLine(p0, p1, colour2, 2.5f); }
                    }
                }

            }
        }

        // DRAW POINTS
        for (int i = 0; i < gNumPoints; i++) {

            Point p = ApplyTransform(&gPointsT[i * gNumDimensions]);


            if (i == 0) { args.DrawingSession().FillCircle(p, 4.0, colour4); }
            else if (i == 8) { args.DrawingSession().FillCircle(p, 4.0, colour3); }
            else { args.DrawingSession().FillCircle(p, 4.0, colour2); }

            args.DrawingSession().DrawCircle(p, 4.0, blueprint, 1.0f);
        }

        return;

        /*
        // DRAW BACKGROUND
        Rect bounds3{ 0.0f, 0.0f, myCanvas().Size().Width, myCanvas().Size().Height };
        CanvasImageBrush ibrush = CanvasImageBrush(sender);
        if (appData->backgroundSoftwareBitmap) {
            ibrush.Image(CanvasBitmap(nullptr).CreateFromSoftwareBitmap(sender, appData->backgroundSoftwareBitmap));
            ibrush.Opacity(255.0f);
            ibrush.ExtendX(CanvasEdgeBehavior::Wrap);
            ibrush.ExtendY(CanvasEdgeBehavior::Wrap);
            args.DrawingSession().FillRectangle(bounds3, ibrush);
        }

        // DRAW WORKING-AREA BORDER
        Rect workAreaBounds = appData->gSelectedDocument->TransformToViewCoordinates(appData->gWorkAreaBounds);
        CanvasStrokeStyle strokeStyle = CanvasStrokeStyle();
        strokeStyle.DashStyle(CanvasDashStyle::Dash);

        Color workAreaColor = Windows::UI::ColorHelper::FromArgb(25, 0, 0, 0);
        args.DrawingSession().FillRectangle(workAreaBounds, workAreaColor);

        Color workAreaBorderColor = Windows::UI::ColorHelper::FromArgb(45, 255, 255, 255);
        args.DrawingSession().DrawRectangle(workAreaBounds, workAreaBorderColor, 2.25, strokeStyle);

        // DRAW WORKING-AREA TEXT
        CanvasTextFormat format;
        format.HorizontalAlignment(CanvasHorizontalAlignment::Center);
        format.VerticalAlignment(CanvasVerticalAlignment::Center);
        format.FontSize(16.0f);
        format.FontFamily(L"Segoe UI Semibold");

        // VECTRASET text
        char buf[60];
        std::string str = "vectraset";
        str += "\n";
        sprintf_s(buf, "%5.1f", appData->gWorkAreaBounds.Width);
        str += buf;
        str += " x ";
        sprintf_s(buf, "%5.1f", appData->gWorkAreaBounds.Height);
        str += buf;
        str += " mm";
        str += "\nmagnification: ";
        sprintf_s(buf, "%5.3f", appData->gSelectedDocument->mViewScale);
        str += buf;

        // DEBUG text
        str += "\n\nposition: (";
        sprintf_s(buf, "%4.2f", appData->gToolPositionX);
        str += buf;
        str += ", ";
        sprintf_s(buf, "%4.2f", appData->gToolPositionY);
        str += buf;
        str += ", ";
        sprintf_s(buf, "%4.2f", appData->gToolPositionZ);
        str += buf;
        str += ")\n";
        str += appData->gSerialReceiveBuffer;


        args.DrawingSession().DrawText(to_hstring(str), workAreaBounds, workAreaBorderColor, format);

        // DRAW ACTIVE DOCUMENT
        if (appData->gSelectedDocument) { appData->gSelectedDocument->Draw_Document(sender, args); }

        // DRAW SELECTION RECTANGLE
        if (gBoundsSelect.X >= 0.0f)
        {
            CanvasStrokeStyle strokeStyle2 = CanvasStrokeStyle();
            strokeStyle2.DashStyle(CanvasDashStyle::Dash);
            gDashOffset -= 0.5f;
            if (gDashOffset < 0.0f) { gDashOffset = 1000.0f; }
            strokeStyle2.DashOffset(gDashOffset);

            Color selectionBorderColor = Windows::UI::ColorHelper::FromArgb(180, 240, 250, 200);
            args.DrawingSession().DrawRectangle(gBoundsSelect, selectionBorderColor, 2.0, strokeStyle2);
        }

        // DRAW TAB TOP BORDER
        // TODO: handle dark mode versus light mode properly
        Color darkModeTabColor = Windows::UI::ColorHelper::FromArgb(255, 40, 40, 40);
        args.DrawingSession().DrawLine(0, 0, CanvasControl_Document().Size().Width, 0, darkModeTabColor, 6);

        // DRAW OFFSCREEN
        //CanvasControl_Draw_Offscreen(appData->gOffscreenRenderTarget);

        */
    }



    // TIMER
    void MainWindow::CreateTimerAndRegisterHandler()
    {
        Microsoft::UI::Xaml::DispatcherTimer timer;
        gTimer = timer;
        gTimer.Interval(std::chrono::milliseconds{ 15 });
        gTimerHandlerRegistrationToken = gTimer.Tick({ this, &MainWindow::OnTick });
    }

    void MainWindow::UnregisterTimerHandler()
    {
        if (gTimer) { gTimer.Tick(gTimerHandlerRegistrationToken); }
    }

    void MainWindow::StartTimer()
    {
        if (gTimer) { gTimer.Start(); }
    }

    void MainWindow::StopTimer()
    {
        if (gTimer) { gTimer.Stop(); }
    }



    void MainWindow::ApplyIncrementAndNormalize(int index, double fraction)
    {
        double length = 0;
        for (int i = 0; i < gNumDimensions; i++)
        {
            gPoints[index * gNumDimensions + i] += fraction * grvec[i];
            length += gPoints[index * gNumDimensions + i] * gPoints[index * gNumDimensions + i];
        }
        length = sqrt(length);

        for (int i = 0; i < gNumDimensions; i++)
        {
            gPoints[index * gNumDimensions + i] /= length;
        }
    }

    void MainWindow::CreateRandomUnitVector()
    {
        //double* res = new double[gNumDimensions];

        double length = 0;
        for (int i = 0; i < gNumDimensions; i++)
        {
            int r = rand() % 100 + 1;   // Generate random number between 1 and 100
            grvec[i] = (double)r / 100.0;
            int s = rand() % 100 + 1;
            if (s < 50) { grvec[i] *= -1.0; }
            length += grvec[i] * grvec[i];
        }
        length = sqrt(length);

        for (int i = 0; i < gNumDimensions; i++)
        {
            grvec[i] /= length;
        }

    }

    void MainWindow::CreateRandomVector()
    {
        //double* res = new double[gNumDimensions];

        //double length = 0;
        for (int i = 0; i < gNumDimensions; i++)
        {
            int r = rand() % 100 + 1;   // Generate random number between 1 and 100
            grvec[i] = 10.0 * (double)r / 100.0;
            int s = rand() % 100 + 1;
            if (s < 50) { grvec[i] *= -1.0; }
            //length += grvec[i] * grvec[i];
        }
        //length = sqrt(length);

        //for (int i = 0; i < gNumDimensions; i++)
        //{
        //    grvec[i] /= length;
        //}

        //return res;
    }

    void MainWindow::ApplyAlgorithm()
    {

        CreateRandomUnitVector();

        // calculate dot product of each point and determine the index of the largest result
        int index = 0;
        double largest = -10000.0;
        for (int i = 0; i < gNumPoints; i++)
        {
            double dot = 0;
            for (int j = 0; j < gNumDimensions; j++)
            {
                dot += grvec[j] * gPoints[i * gNumDimensions + j];
            }

            if (dot > largest)
            {
                largest = dot;
                index = i;
            }
        }

        // ****** TEST ******
        //CreateDynamicTopologyList();

        //double inc = 0.005 - ((double)gTickCount) / 100000.0;
        //if (inc < 0.000001) { inc = 0.000001; }

        ApplyIncrementAndNormalize(index, 0.0005);
        //ApplyIncrementAndNormalize(index, inc);


        if(gFollowTopology)
        {
            if (gNumDimensions >= 3)
            {
                for (int i = 0; i < gNumDimensions; i++)
                //for (int i = 0; i < gNumDimensions * 2; i++)
                    {
                    ApplyIncrementAndNormalize(gTopology[index * gNumDimensions + i], 0.0002);
                    //ApplyIncrementAndNormalize(gTopology[index * gNumDimensions + i], inc / 2);
                    //ApplyIncrementAndNormalize(gTopology[index * gNumDimensions * 2 + i], 0.0001);
                }
            }
        }


    }

    arma::mat Rotate(int dims, int a1, int a2, double rads)
    {
        arma::mat A = arma::eye<arma::mat>(dims, dims);     // create identity
        double c = cos(rads);
        double s = sin(rads);
        A(a1, a1) = c;
        A(a1, a2) = -s;
        A(a2, a1) = s;
        A(a2, a2) = c;

        return A;
    }


    void MainWindow::OnTick(IInspectable const& sender, IInspectable const& args)
    {
        gTheta += gThetaInc;
        if (gTheta > 1.0 * 6.28) { gThetaInc = 0.0; }

        gTickCount += 1;
        if (gTickCount < 30000)
        {
            // ****** TEST ******
            //CreateDynamicTopologyList();

            for (int i = 0; i < 2000; i++)
            {
                ApplyAlgorithm();
            }
        }

        mat B(gPoints, gNumDimensions, gNumPoints);

        // Z-AXIS 45 degree = Rotate(gNumDimensions,0,1,3.14/4.0)
        //mat C = Rotate(gNumDimensions, 0, 3, gTheta) * Rotate(gNumDimensions, 1, 3, gTheta) * Rotate(gNumDimensions, 2, 3, gTheta) * Rotate(gNumDimensions, 0, 2, -3.14 / 4.0) * Rotate(gNumDimensions, 1, 2, 3.14 / 4.0) * B;
        mat C;
        if(gNumDimensions >= 4)
        {
            C = Rotate(gNumDimensions, 2, 3, gTheta) * Rotate(gNumDimensions, 0, 2, 3.14 * ((gX + gdX) / 180.0)) * Rotate(gNumDimensions, 1, 2, 3.14 * ((gY + gdY) / 180)) * B;
            //C = Rotate(gNumDimensions, 0, 2, 3.14 * ((gX + gdX) / 180.0)) * Rotate(gNumDimensions, 1, 2, 3.14 * ((gY + gdY) / 180)) * B;
        }
        else if (gNumDimensions == 3)
        {
            C = Rotate(gNumDimensions, 0, 2, 3.14 * ((gX + gdX) / 180.0)) * Rotate(gNumDimensions, 1, 2, 3.14 * ((gY + gdY) / 180)) * B;
        }
        else
        {
            C = Rotate(gNumDimensions, 0, 1, 3.14 * ((gX + gdX) / 180.0)) * B;
        }
        //        mat C = Rotate(gNumDimensions, 1, 3, gTheta) * B;

        for (int i = 0; i < gNumPoints; i++)
        {
            for (int j = 0; j < gNumDimensions; j++)
            {
                gPointsT[i * gNumDimensions + j] = C(j, i);
            }

        }



        // FLAG FOR REDRAW
        myCanvas().Invalidate();
    }

    void MainWindow::DEBUG_ShowValues()
    {
        // DEBUG: show point values
        std::string str = "";
        for (int i = 0; i < gNumPoints; i++)
        {
            //m += "\n\r";

            char buf[60];
            for (int j = 0; j < gNumDimensions; j++)
            {
                sprintf_s(buf, "%1.3f", gPoints[i * gNumDimensions + j]);
                str += buf;
                str += " ";
            }
            str += "\n\r";
        }
        std::wstring s1 = string_to_wstring(str);
        std::wstring s3 = L"Vertex Array Values";
        MessageBox(nullptr, s1.c_str(), s3.c_str(), MB_OK);
    }

    void MainWindow::DEBUG_ShowValues2()
    {
        // DEBUG: show point values
        std::string str = "";
        for (int i = 0; i < gNumPoints; i++)
        {
            //m += "\n\r";

            char buf[60];
            for (int j = 0; j < gNumDimensions; j++)
            {
                sprintf_s(buf, "%i", gTopology[i * gNumDimensions + j]);
                str += buf;
                str += " ";
            }
            str += "\n\r";
        }
        std::wstring s1 = string_to_wstring(str);
        std::wstring s3 = L"Topology Array Values";
        MessageBox(nullptr, s1.c_str(), s3.c_str(), MB_OK);
    }

    std::wstring MainWindow::string_to_wstring(const std::string& text) {
        return std::wstring(text.begin(), text.end());
    }


    void MainWindow::CanvasControl_PointerPressed(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::Input::PointerRoutedEventArgs const& args)
    {

        // IMPORTANT SYNTAX: converts sender into a useable format
        auto cc{ sender.as<CanvasControl>() };

        winrt::Microsoft::UI::Input::PointerPoint ptrPt = args.GetCurrentPoint(cc.as<UIElement>());
        PointerPointProperties prop = ptrPt.Properties();
        Point p = ptrPt.Position();

        if (args.Pointer().PointerDeviceType() == PointerDeviceType::Mouse)
        {

            if (!gMouseDownLeft && prop.IsLeftButtonPressed())
            {
                gX0 = p.X;
                gY0 = p.Y;
                gdX = 0;
                gdY = 0;
                gMouseDownLeft = true;
                gMouseMoved = false;


                //std::string str = "(";
                //str += std::to_string(p.X);
                //str += ",";
                //str += std::to_string(p.Y);
                //str += ")";
                //DEBUG_SimpleMessage(str);

            }
            if (prop.IsMiddleButtonPressed())
            {
                //appData->gX = p.X;
                //appData->gY = p.Y;
                //appData->gMouseDownMiddle = true;
                //appData->gMouseMoved = false;
            }
            if (prop.IsRightButtonPressed())
            {
            }
        }

        // Prevent most handlers along the event route from handling the same event again.
        args.Handled(true);
    }

    void MainWindow::CanvasControl_PointerReleased(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::Input::PointerRoutedEventArgs const& args)
    {

        // IMPORTANT SYNTAX: converts sender into a useable format
        auto cc{ sender.as<CanvasControl>() };

        winrt::Microsoft::UI::Input::PointerPoint ptrPt = args.GetCurrentPoint(cc.as<UIElement>());
        PointerPointProperties prop = ptrPt.Properties();
        Point p = ptrPt.Position();

        gMouseDownLeft = false;
        //gMouseDownMiddle = false;
        gMouseMoved = false;
        gX += gdX;
        gY += gdY;
        gdX = 0;
        gdY = 0;

        // TODO: release mouse capture
        args.Handled(true);
    }


    void MainWindow::CanvasControl_PointerMoved(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::Input::PointerRoutedEventArgs const& args)
    {

        // IMPORTANT SYNTAX: converts sender into a useable format
        auto cc{ sender.as<CanvasControl>() };

        winrt::Microsoft::UI::Input::PointerPoint ptrPt = args.GetCurrentPoint(cc.as<UIElement>());
        PointerPointProperties prop = ptrPt.Properties();
        Point p = ptrPt.Position();

        //gRoamingMouse = p;             // save the latest mouse coordinates

        if (args.Pointer().PointerDeviceType() == winrt::Microsoft::UI::Input::PointerDeviceType::Mouse)
        {

            if (gMouseDownLeft)
            {
                // MOVE PARTS
                Point dp;
                dp.X = p.X - gX0;
                dp.Y = p.Y - gY0;

                if ((abs(dp.X) > 0) || (abs(dp.Y) > 0)) { gMouseMoved = true; }

                gdX = dp.X;
                gdY = dp.Y;

                args.Handled(true);
            }
            //else if (gMouseDownMiddle)
            //{
            //
            //}
            
        }
    }

    void MainWindow::NumberBox_NumberOfDimensions_ValueChanged(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::Controls::NumberBoxValueChangedEventArgs const& args)
    {
        auto nb{ sender.as<NumberBox>() };

        double newval = args.NewValue();
        if (newval < 2.0) { newval = 2.0; }               // min
        if (newval > 100.0) { newval = 100.0; }           // max
        nb.Value(floor(newval));

        double val = nb.Value();
        gNumDimensions = (int)val;
        Reset();
    }

    void MainWindow::NumberBox_NumberOfPoints_ValueChanged(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::Controls::NumberBoxValueChangedEventArgs const& args)
    {
        auto nb{ sender.as<NumberBox>() };

        double newval = args.NewValue();
        if (newval < 1.0) { newval = 1.0; }               // min
        nb.Value(floor(newval));

        double val = nb.Value();
        gNumPoints = (int)val;
        Reset();
    }


    void MainWindow::Slider_VisualDepth_ValueChanged(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs const& args)
    {
        auto slider{ sender.as<Slider>() };

        double newval = args.NewValue();
        gDepthScale = newval/100.0;
    }

}
