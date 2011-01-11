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

#ifndef SIMPLEWIREENGINE_H
#define SIMPLEWIREENGINE_H

#include <avogadro/global.h>
#include <avogadro/extension.h>

namespace Avogadro {

class VTKDialog;

//! VTK Extension class.
class VTKExtension : public Extension
{
  Q_OBJECT
  AVOGADRO_EXTENSION("VTK",
                  tr("VTK"),
                  tr("Use VTK to render and analyse molecular data"))

public:
  //! Constructor
  VTKExtension(QObject *parent=0);
  //! Destructor
  virtual ~VTKExtension();

  /**
   * @return a list of actions which this extension can perform
   */
  virtual QList<QAction *> actions() const;

  /**
   * @return the menu path for the specified action
   */
  virtual QString menuPath(QAction *action) const;

  /**
   * @param action the action that triggered the calls
   * @param widget the currently active GLWidget
   * feedback to the user)
   * @return an undo command for this action
   */
  virtual QUndoCommand* performAction(QAction *action, GLWidget *widget);

  /**
   * save settings for this extension
   * @param settings settings variable to write settings to
   */
  virtual void writeSettings(QSettings &settings) const;

  /**
   * read settings for this extension
   * @param settings settings variable to read settings from
   */
  virtual void readSettings(QSettings &settings);

  /**
   * Determines the ordering of the extensions.  More useful
   * extensions will be placed first in menus. It is up to the
   * extension designer to be humble about their usefulness value.
   * @return usefulness value
   */
  virtual int usefulness() const;

public slots:
  /**
   * Slot to set the Molecule for the Extension - should be called whenever
   * the active Molecule changes.
   */
  virtual void setMolecule(Molecule *molecule);

protected:
  QList<QAction *> m_actions;
  VTKDialog *m_vtkDialog;

};

//! Generates instances of our WireEngine class
class VTKExtensionFactory : public QObject, public PluginFactory
{
  Q_OBJECT
  Q_INTERFACES(Avogadro::PluginFactory)
  AVOGADRO_EXTENSION_FACTORY(VTKExtension)
};

} // end namespace Avogadro

#endif

