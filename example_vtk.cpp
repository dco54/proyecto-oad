#include "vtkCylinderSource.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkProperty.h"
#include "vtkCamera.h"
#include "vtkSmartPointer.h"
 
int main()
{
  vtkSmartPointer<vtkCylinderSource> cylinder =
    vtkSmartPointer<vtkCylinderSource>::New();
  cylinder->SetResolution(8);
 
  vtkSmartPointer<vtkPolyDataMapper> cylinderMapper =
    vtkSmartPointer<vtkPolyDataMapper>::New();
    
  cylinderMapper->SetInputConnection(cylinder->GetOutputPort());
 
  vtkSmartPointer<vtkActor> cylinderActor =
    vtkSmartPointer<vtkActor>::New();
    
  cylinderActor->SetMapper(cylinderMapper);
  cylinderActor->GetProperty()->SetColor(1.0000, 0.3882, 0.2784);
  cylinderActor->RotateX(30.0);
  cylinderActor->RotateY(-45.0);
 
  vtkSmartPointer<vtkRenderer> renderer =
    vtkSmartPointer<vtkRenderer>::New();
  renderer->AddActor(cylinderActor);
  renderer->SetBackground(0.1, 0.2, 0.4);
  renderer->ResetCamera();
  renderer->GetActiveCamera()->Zoom(1.5);
 
  vtkSmartPointer<vtkRenderWindow> renderWindow =
    vtkSmartPointer<vtkRenderWindow>::New();
  renderWindow->SetSize(200, 200);
  renderWindow->AddRenderer(renderer);
 
  vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
    vtkSmartPointer<vtkRenderWindowInteractor>::New();
  renderWindowInteractor->SetRenderWindow(renderWindow);
 
  renderWindowInteractor->Start();
 
  return 0;
}
