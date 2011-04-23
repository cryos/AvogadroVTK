/******************************************************************************

  This source file is part of the Avogadro project.

  Copyright Kitware, Inc.

  This source code is released under the New BSD License, (the "License").

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

******************************************************************************/

#include "VTKDialog.h"

// Qt includes
#include <QVBoxLayout>
#include <QDebug>

// Avogadro includes
#include <avogadro/glwidget.h>
#include <avogadro/molecule.h>
#include <avogadro/atom.h>
#include <avogadro/bond.h>
#include <avogadro/cube.h>

// VTK includes
#include <QVTKWidget.h>
#include <vtkRenderViewBase.h>
#include <vtkRenderer.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkSmartVolumeMapper.h>
#include <vtkVolume.h>
#include <vtkColorTransferFunction.h>
#include <vtkPiecewiseFunction.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkVolumeProperty.h>
#include <vtkImageShiftScale.h>
#include <vtkImageData.h>
#include <vtkLookupTable.h>
#include <vtkPolyData.h>
#include <vtkIntArray.h>
#include <vtkFloatArray.h>
#include <vtkPoints.h>
#include <vtkPointData.h>
#include <vtkCellArray.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>

// For the atom colors...
#include <openbabel/mol.h>

namespace Avogadro
{

VTKDialog::VTKDialog(QWidget* parent, Qt::WindowFlags f)
  : QDialog(parent, f)
{
  m_qvtkWidget = new QVTKWidget(this);
  m_context->SetInteractor(m_qvtkWidget->GetInteractor());
  m_qvtkWidget->SetRenderWindow(m_context->GetRenderWindow());

  vtkInteractorStyleTrackballCamera *interactor =
      vtkInteractorStyleTrackballCamera::New();
  m_context->GetInteractor()->SetInteractorStyle(interactor);

  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->addWidget(m_qvtkWidget);
  setLayout(layout);

  // Some initialization
  lut();
}

VTKDialog::~VTKDialog()
{
  m_qvtkWidget->deleteLater();
}

void VTKDialog::setMolecule(Molecule *mol)
{
  if (!mol || mol->numCubes() == 0) {
    return;
  }
  moleculePolyData(mol);

  Cube *cube = mol->cube(0);

  vtkVolume *volume = cubeVolume(cube);
  m_context->GetRenderer()->AddViewProp(volume);

  vtkNew<vtkPolyDataMapper> m;
  m->SetInput(m_moleculePolyData.GetPointer());
  m->SetScalarRange(0, 106);
  m->SetLookupTable(m_lut.GetPointer());
  m->SetScalarModeToUsePointFieldData();
  m->ColorByArrayComponent("z", 0);

  vtkNew<vtkActor> actor;
  actor->SetMapper(m.GetPointer());
  actor->GetProperty()->SetLineWidth(3);
  actor->GetProperty()->SetPointSize(5);

  m_context->GetRenderer()->AddViewProp(actor.GetPointer());

  m_context->GetRenderer()->ResetCamera();
}

vtkVolume * VTKDialog::cubeVolume(Cube *cube)
{
  qDebug() << "Cube dimensions: " << cube->dimensions().x()
           << cube->dimensions().y() << cube->dimensions().z();

  qDebug() << "min/max:" << cube->minValue() << cube->maxValue();
  qDebug() << cube->data()->size();

  vtkImageData *data = vtkImageData::New();
  data->SetNumberOfScalarComponents(1);
  Eigen::Vector3i dim = cube->dimensions();
  data->SetExtent(0, dim.x()-1, 0, dim.y()-1, 0, dim.z()-1);

  data->SetOrigin(cube->min().x(), cube->min().y(), cube->min().z());
  data->SetSpacing(cube->spacing().data());

  data->SetScalarTypeToDouble();
  data->AllocateScalars();
  data->Update();

  double *dataPtr = static_cast<double *>(data->GetScalarPointer());
  std::vector<double> *cubePtr = cube->data();

  for (int i = 0; i < dim.x(); ++i)
    for (int j = 0; j < dim.y(); ++j)
      for (int k = 0; k < dim.z(); ++k) {
        dataPtr[(k * dim.y() + j) * dim.x() + i] =
            (*cubePtr)[(i * dim.y() + j) * dim.z() + k];
      }

  double range[2];
  data->Update();
  range[0] = data->GetScalarRange()[0];
  range[1] = data->GetScalarRange()[1];
//  a->GetRange(range);
  qDebug() << "ImageData range: " << range[0] << range[1];

  vtkImageShiftScale *t = vtkImageShiftScale::New();
  t->SetInput(data);
  t->SetShift(-range[0]);
  double magnitude = range[1] - range[0];
  if(magnitude == 0.0)
    {
    magnitude = 1.0;
    }
  t->SetScale(255.0/magnitude);
  t->SetOutputScalarTypeToDouble();

  qDebug() << "magnitude: " << magnitude;

  t->Update();

  vtkSmartVolumeMapper *volumeMapper;
  vtkVolumeProperty *volumeProperty;
  vtkVolume *volume;

  volumeMapper=vtkSmartVolumeMapper::New();
  volumeMapper->SetBlendModeToComposite();
//  volumeMapper->SetBlendModeToComposite(); // composite first
  volumeMapper->SetInputConnection(t->GetOutputPort());

  volumeProperty=vtkVolumeProperty::New();
  volumeProperty->ShadeOff();
  volumeProperty->SetInterpolationTypeToLinear();

  vtkPiecewiseFunction *compositeOpacity = vtkPiecewiseFunction::New();
  compositeOpacity->AddPoint(  0.00, 0.0);
  compositeOpacity->AddPoint( 63.75, 0.5);
  compositeOpacity->AddPoint(127.50, 0.0);
  compositeOpacity->AddPoint(192.25, 0.5);
  compositeOpacity->AddPoint(255.00, 0.0);
  volumeProperty->SetScalarOpacity(compositeOpacity); // composite first.

  vtkColorTransferFunction *color=vtkColorTransferFunction::New();
  color->AddRGBPoint(  0.00, 0.0, 0.0, 0.0);
  color->AddRGBPoint( 63.75, 1.0, 0.0, 0.0);
  color->AddRGBPoint(127.50, 0.0, 0.2, 0.0);
  color->AddRGBPoint(191.25, 0.0, 0.0, 1.0);
  color->AddRGBPoint(255.00, 0.0, 0.0, 0.0);
  volumeProperty->SetColor(color);
  color->Delete();

  volume = vtkVolume::New();
  volume->SetMapper(volumeMapper);
  volume->SetProperty(volumeProperty);

  return volume;
}

void VTKDialog::moleculePolyData(Molecule *mol)
{
  unsigned int nAtoms = mol->numAtoms();
  vtkNew<vtkIntArray> aNums;
  aNums->SetName("z");
  aNums->SetNumberOfComponents(1);
  aNums->SetNumberOfTuples(nAtoms);
  vtkNew<vtkFloatArray> radius;
  radius->SetName("sizevecs");
  radius->SetNumberOfComponents(3);
  radius->SetNumberOfTuples(nAtoms);
  vtkNew<vtkPoints> pts;
  pts->SetNumberOfPoints(nAtoms);

  // Atoms
  std::vector<unsigned int> orphans;
  for(unsigned int i = 0; i < nAtoms; ++i) {
    Atom *a = mol->atom(i);
    const Eigen::Vector3d *p = a->pos();
    aNums->SetValue(i, a->atomicNumber());
    double r = OpenBabel::etab.GetVdwRad(a->atomicNumber());
    radius->SetTuple3(i, r, r, r);
    pts->SetPoint(i, p->data());

    if (a->bonds().size() == 0)
      orphans.push_back(i);
  }

  // Bonds
  vtkNew<vtkCellArray> bonds;
  QList<Bond *> bList = mol->bonds();
  bonds->Allocate(bList.size(), bList.size());
  foreach(Bond *bond, bList) {
    bonds->InsertNextCell(2);
    bonds->InsertCellPoint(bond->beginAtomId());
    bonds->InsertCellPoint(bond->endAtomId());
  }
  vtkNew<vtkCellArray> verts;
  verts->Allocate(orphans.size(), orphans.size());
  foreach(unsigned int o, orphans) {
    verts->InsertNextCell(1);
    verts->InsertCellPoint(o);
  }

  m_moleculePolyData->SetPoints(pts.GetPointer());
  m_moleculePolyData->SetLines(bonds.GetPointer());
  m_moleculePolyData->SetVerts(verts.GetPointer());

  m_moleculePolyData->GetPointData()->SetScalars(aNums.GetPointer());
  m_moleculePolyData->GetPointData()->SetVectors(radius.GetPointer());
}

void VTKDialog::lut()
{
  int n = 106;
  m_lut->SetNumberOfColors(n);
  std::vector<double> rgb;
  for(int i = 0; i < n; ++i) {
    rgb = OpenBabel::etab.GetRGB(i);
    m_lut->SetTableValue(i, rgb[0], rgb[1], rgb[2]);
  }
}

}

#include "VTKDialog.moc"
