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

#include "vtkextension.h"

#include <QAction>

#include <avogadro/molecule.h>
#include <avogadro/glwidget.h>

#include "VTKDialog.h"

namespace Avogadro {

using Eigen::Vector3d;

VTKExtension::VTKExtension(QObject *parent) : Extension(parent), m_vtkDialog(0)
{
  QAction* action = new QAction(this);
  action->setText(tr("VTK..."));
  m_actions.append(action);
}

VTKExtension::~VTKExtension()
{
}

QList<QAction *> VTKExtension::actions() const
{
  return m_actions;
}

QString VTKExtension::menuPath(QAction *action) const
{
  return tr("E&xtensions");
}

QUndoCommand* VTKExtension::performAction(QAction *action, GLWidget *widget)
{
  if (!m_vtkDialog) {
    m_vtkDialog = new VTKDialog(qobject_cast<QWidget *>(this->parent()));
    m_vtkDialog->setGeometry(0, 0, 800, 600);
  }
  m_vtkDialog->show();
  m_vtkDialog->setMolecule(widget->molecule());
  return 0;
}

void VTKExtension::writeSettings(QSettings &settings) const
{

}

void VTKExtension::readSettings(QSettings &settings)
{

}

int VTKExtension::usefulness() const
{
  return 5000;
}

void VTKExtension::setMolecule(Molecule *molecule)
{

}

} // End namespace

#include "vtkextension.moc"

Q_EXPORT_PLUGIN2(vtkextension, Avogadro::VTKExtensionFactory)
