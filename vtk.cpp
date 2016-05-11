#include <vtkVersion.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderWindow.h>
#include <vtkSmartPointer.h>
#include <vtkFloatArray.h>
#include <vtkPlaneSource.h>
#include <vtkMath.h>
#include <vtkPoints.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkDataSetMapper.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkTransform.h>
#include <vtkProperty.h>
#include <vtkStructuredGrid.h>
#include <vtkCubeAxesActor2D.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkInteractorStyleTrackball.h>
#include <vtkCommand.h>
#include <vtkTimerLog.h>
#include <vtkCallbackCommand.h>
#include <vtkDelaunay2D.h>
#include <vtkXMLPolyDataWriter.h>
#include <vtkLookupTable.h>
#include <vtkElevationFilter.h>
#include <vtkCommand.h>
#include "gauss.h"
#include "vtkWarpScalar.h"

static void CallbackFunction(vtkObject* caller, long unsigned int eventId, void* clientData, void* callData);

class vtkUpdatePlot : public vtkCommand
{
public:
  vtkUpdatePlot(vtkSmartPointer<vtkRenderWindow> _renderWindow, vtkSmartPointer<vtkPolyData> _input, vtkSmartPointer<vtkDataSetMapper> _mapper, vtkSmartPointer<vtkWarpScalar> _warp, double _amplitude, std::vector<double> _sigmas, std::vector<double> _centers, std::vector<unsigned int> _cv_dims) : m_renderWindow(_renderWindow), m_input(_input), m_mapper(_mapper), m_warp(_warp), m_amplitude(_amplitude), m_sigmas(_sigmas), m_centers(_centers), m_cv_dims(_cv_dims) {}
  static vtkUpdatePlot* New(vtkSmartPointer<vtkRenderWindow> _renderWindow, vtkSmartPointer<vtkPolyData> _input, vtkSmartPointer<vtkDataSetMapper> _mapper, vtkSmartPointer<vtkWarpScalar> _warp, double _amplitude, std::vector<double> _sigmas, std::vector<double> _centers, std::vector<unsigned int> _cv_dims) { return new vtkUpdatePlot(_renderWindow, _input, _mapper, _warp, _amplitude, _sigmas, _centers, _cv_dims); }
  virtual void Execute(vtkObject *caller, unsigned long eventId,
                       void *callData)
  {
     timespec start, b4_render, start_gauss, end_gauss, start_conv, end_conv, start_poly, end_poly, end;
     clock_gettime(CLOCK_MONOTONIC, &start);
     vtkRenderWindowInteractor *interactor = vtkRenderWindowInteractor::SafeDownCast(caller);

     //calculate new gaussian
     m_amplitude += 0.1;

     cv::Mat surface = gaussMatrix(2, m_cv_dims, m_amplitude, m_sigmas, m_centers, true);

     //warp plane according to cv::Mat surface
     int numPts = m_cv_dims.at(0) * m_cv_dims.at(1);
     vtkPoints *newPts = vtkPoints::New();
     newPts->SetNumberOfPoints(numPts);
     vtkSmartPointer<vtkFloatArray> z_scalars = vtkSmartPointer<vtkFloatArray>::New();
     z_scalars->SetNumberOfTuples(numPts);

     m_input->SetPoints(newPts);
     m_input->GetPointData()->SetScalars(z_scalars);

     int point_number = 0;
     double point[3];
     for(int i=0; i<m_cv_dims.at(0); i++)
     {
       for(int j=0; j<m_cv_dims.at(1); j++)
       {
         //InsertNextPoint(x,y,z)
         point[0] = i;
         point[1] = j;
         point[2] = surface.at<float>(i,j);
         newPts->SetPoint(point_number, point);
         z_scalars->SetValue(point_number, surface.at<float>(i,j));
         point_number++;
       }
     }

     // warp plane
     //vtkWarpScalar *warp = vtkWarpScalar::New();
     m_warp->SetInput(m_input);
     m_warp->XYPlaneOn();
     m_warp->SetScaleFactor(0.0);

     // Visualize
     m_mapper->SetInputConnection(m_warp->GetOutputPort());
     double tmp[2];
     m_input->GetScalarRange(tmp);
     m_mapper->SetScalarRange(tmp[0],tmp[1]);

     m_warp->Modified();
     m_mapper->Modified();
     m_renderWindow->Render();

     interactor->CreateTimer(VTKI_TIMER_UPDATE);
  }

private:
  vtkSmartPointer<vtkRenderWindow> m_renderWindow;
  vtkSmartPointer<vtkPolyData> m_input;
  vtkSmartPointer<vtkDataSetMapper> m_mapper;
  vtkSmartPointer<vtkWarpScalar> m_warp;
  double m_amplitude;
  std::vector<double> m_sigmas;
  std::vector<double> m_centers;
  std::vector<unsigned int> m_cv_dims;
};


int main(int, char *[])
{
  int base = 100;
  std::vector<unsigned int> cv_dims(3);
  std::vector<unsigned int>::iterator uit;
  uit = cv_dims.begin();
  uit = cv_dims.insert(uit, base);
  cv_dims.insert(uit, base);

  double amplitude = base/2.0;
  std::vector<double> sigmas(2);
  std::vector<double>::iterator dit;
  dit = sigmas.begin();
  dit = sigmas.insert(dit, base/3);
  sigmas.insert(dit, base/3);
  std::vector<double> centers(2);
  dit = centers.begin();
  dit = centers.insert(dit, base/2);
  centers.insert(dit, base/2);

  // create plane to warp
  vtkSmartPointer<vtkPlaneSource> plane = vtkSmartPointer<vtkPlaneSource>::New();
  plane->SetResolution (base-1,base-1);

  vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
  transform->Scale(10.0,10.0,1.0);

  vtkSmartPointer<vtkTransformPolyDataFilter> transF = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
  transF->SetInputConnection(plane->GetOutputPort());
  transF->SetTransform(transform);
  transF->Update();

  //warp plane according to cv::Mat surface
  vtkSmartPointer<vtkPolyData> input = transF->GetOutput();
  int numPts = input->GetNumberOfPoints();

  vtkSmartPointer<vtkPoints> newPts = vtkSmartPointer<vtkPoints>::New();
  newPts->SetNumberOfPoints(numPts);
  vtkSmartPointer<vtkFloatArray> z_scalars = vtkSmartPointer<vtkFloatArray>::New();
  z_scalars->SetNumberOfTuples(numPts);

  vtkSmartPointer<vtkPolyData> inputPolyData =
    vtkSmartPointer<vtkPolyData>::New();
  inputPolyData->CopyStructure(input);
  inputPolyData->SetPoints(newPts);
  inputPolyData->GetPointData()->SetScalars(z_scalars);

  //Mat(int _rows, int _cols, int _type)
  cv::Mat surface = gaussMatrix(2, cv_dims, amplitude, sigmas, centers, true);

  int point_number = 0;
  double point[3];
  for(int i=0; i<cv_dims.at(0); i++)
  {
    for(int j=0; j<cv_dims.at(1); j++)
    {
      //InsertNextPoint(x,y,z)
      point[0] = i;
      point[1] = j;
      point[2] = surface.at<float>(i,j);
      newPts->SetPoint(point_number, point);
      z_scalars->SetValue(point_number, surface.at<float>(i,j));
      point_number++;
    }
  }

  // warp plane
  vtkSmartPointer<vtkWarpScalar> warp = vtkSmartPointer<vtkWarpScalar>::New();
  warp->SetInput(inputPolyData);
  warp->XYPlaneOn();
  warp->SetScaleFactor(0.0);

  // Visualize
  vtkSmartPointer<vtkDataSetMapper> mapper =
    vtkSmartPointer<vtkDataSetMapper>::New();
  mapper->SetInputConnection(warp->GetOutputPort());

  double tmp[2];
  inputPolyData->GetScalarRange(tmp);
  mapper->SetScalarRange(tmp[0],tmp[1]);

  vtkSmartPointer<vtkActor> actor =
    vtkSmartPointer<vtkActor>::New();
  actor->SetMapper(mapper);

  vtkSmartPointer<vtkRenderer> renderer =
    vtkSmartPointer<vtkRenderer>::New();
  vtkSmartPointer<vtkRenderWindow> renderWindow =
    vtkSmartPointer<vtkRenderWindow>::New();
  renderWindow->AddRenderer(renderer);
  vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
    vtkSmartPointer<vtkRenderWindowInteractor>::New();
  renderWindowInteractor->SetRenderWindow(renderWindow);

  renderer->AddActor(actor);
  renderer->SetBackground(.3, .6, .3);

  //FrameRate
  vtkSmartPointer<vtkCallbackCommand> callback = vtkSmartPointer<vtkCallbackCommand>::New();
  callback->SetCallback(CallbackFunction);
  renderer->AddObserver(vtkCommand::EndEvent, callback);

  renderWindow->Render();

  vtkSmartPointer<vtkInteractorStyleTrackballCamera> style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
  renderWindowInteractor->SetInteractorStyle(style);

  // add & render CubeAxes
  vtkSmartPointer<vtkCubeAxesActor2D> axes = vtkSmartPointer<vtkCubeAxesActor2D>::New();
  axes->SetInput(warp->GetOutput());
  //axes->SetBounds(0.0, 10.0, 0.0, 10.0, 0.0, 10.0);
  axes->SetFontFactor(3.0);
  axes->SetInertia(0);
  axes->SetFlyModeToNone();
  //axes->GetProperty()->SetColor(1.0,1.0,1.0);
  axes->SetCamera(renderer->GetActiveCamera());

  renderer->AddViewProp(axes);

  vtkUpdatePlot *cmd = vtkUpdatePlot::New(renderWindow, inputPolyData, mapper, warp, 30.0, sigmas, centers, cv_dims);
  renderWindowInteractor->CreateTimer(VTKI_TIMER_FIRST);
  renderWindowInteractor->AddObserver(vtkCommand::TimerEvent, cmd);

  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}

void CallbackFunction(vtkObject* caller, long unsigned int vtkNotUsed(eventId), void* clientData, void* vtkNotUsed(callData))
{
  vtkRenderer* renderer = static_cast<vtkRenderer*>(caller);
  double timeInSeconds = renderer->GetLastRenderTimeInSeconds();
  double fps = 1.0/timeInSeconds;
  std::cout << "Rendering FPS: " << fps << std::endl;
}
