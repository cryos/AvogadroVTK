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

#ifndef VTKDIALOG_H
#define VTKDIALOG_H

#include <QDialog>

#include <vtkSmartPointer.h>
#include <vtkNew.h>

class QComboBox;

class QVTKWidget;
class vtkRenderViewBase;
class vtkVolume;
class vtkLookupTable;
class vtkPolyData;
class vtkVolume;

namespace Avogadro
{

class GLWidget;
class Molecule;
class Cube;

class VTKDialog : public QDialog
{
  Q_OBJECT

public:
  explicit VTKDialog(QWidget* parent = 0, Qt::WindowFlags f = 0);
  ~VTKDialog();

  void setMolecule(Molecule *mol);

protected slots:
  void cubeChanged(int index);

protected:
  vtkVolume * cubeVolume(Cube *cube);
  void moleculePolyData(Molecule *mol);
  void lut();

  void updateCubeCombo();

private:
  const GLWidget *m_glwidget;
  const Molecule *m_molecule;
  QVTKWidget *m_qvtkWidget;
  vtkNew<vtkRenderViewBase> m_context;
  vtkNew<vtkLookupTable> m_lut;
  vtkNew<vtkPolyData> m_moleculePolyData;
  vtkSmartPointer<vtkVolume> m_volume;

  QComboBox *m_comboCube;
};

}

#endif // VTKDIALOG_H
