/******************************************************************************

  This source file is part of the Avogadro project.

  Copyright 2011 Kitware, Inc.

  This source code is released under the New BSD License, (the "License").

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

******************************************************************************/

#include "VTKDialog.h"

#include <QVBoxLayout>

#include <avogadro/glwidget.h>
#include <avogadro/molecule.h>

#include <QVTKWidget.h>
#include <vtkContextView.h>
#include <vtkRenderViewBase.h>
#include <vtkContextScene.h>
#include <vtkBlockItem.h>

#include <vtkSphere.h>
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"
#include "vtkActor.h"
#include "vtkCamera.h"
#include "vtkPlaneSource.h"
#include "vtkElevationFilter.h"
#include "vtkPolyDataMapper.h"
#include "vtkProperty.h"
#include "vtkSuperquadricSource.h"
#include "vtkGlyph3DMapper.h"
#include "vtkInteractorStyleTrackballCamera.h"

namespace Avogadro
{

VTKDialog::VTKDialog(QWidget* parent, Qt::WindowFlags f)
  : QDialog(parent, f)
{
  m_qvtkWidget = new QVTKWidget(this);
  vtkRenderViewBase *context = vtkRenderViewBase::New();
  context->SetInteractor(m_qvtkWidget->GetInteractor());
  m_qvtkWidget->SetRenderWindow(context->GetRenderWindow());

  int res=6;
  vtkPlaneSource *plane=vtkPlaneSource::New();
  plane->SetResolution(res,res);
  vtkElevationFilter *colors=vtkElevationFilter::New();
  colors->SetInputConnection(plane->GetOutputPort());
  plane->Delete();
  colors->SetLowPoint(-0.25,-0.25,-0.25);
  colors->SetHighPoint(0.25,0.25,0.25);
  vtkPolyDataMapper *planeMapper=vtkPolyDataMapper::New();
  planeMapper->SetInputConnection(colors->GetOutputPort());
  colors->Delete();

  vtkActor *planeActor=vtkActor::New();
  planeActor->SetMapper(planeMapper);
  planeMapper->Delete();
  planeActor->GetProperty()->SetRepresentationToWireframe();

// create simple poly data so we can apply glyph
  vtkSuperquadricSource *squad=vtkSuperquadricSource::New();

  vtkGlyph3DMapper *glypher=vtkGlyph3DMapper::New();
  glypher->SetInputConnection(colors->GetOutputPort());
  glypher->SetSourceConnection(squad->GetOutputPort());
  squad->Delete();
  vtkActor *glyphActor=vtkActor::New();
  glyphActor->SetMapper(glypher);
  glypher->Delete();

  context->GetRenderer()->AddActor(glyphActor);

  vtkInteractorStyleTrackballCamera *interactor = vtkInteractorStyleTrackballCamera::New();
  context->GetInteractor()->SetInteractorStyle(interactor);

  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->addWidget(m_qvtkWidget);
  setLayout(layout);
}

VTKDialog::~VTKDialog()
{
  m_qvtkWidget->deleteLater();
}

}

#include "VTKDialog.moc"
