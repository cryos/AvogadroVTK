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

class QVTKWidget;
class vtkRenderViewBase;

namespace Avogadro
{

class GLWidget;
class Molecule;

class VTKDialog : public QDialog
{
  Q_OBJECT

public:
  explicit VTKDialog(QWidget* parent = 0, Qt::WindowFlags f = 0);
  ~VTKDialog();

  void setMolecule(Molecule *mol);

private:
  const GLWidget *m_glwidget;
  const Molecule *m_molecule;
  QVTKWidget *m_qvtkWidget;
  vtkRenderViewBase *m_context;
};

}

#endif // VTKDIALOG_H
