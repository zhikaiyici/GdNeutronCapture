//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
/// \file exampleGdNCap.cc
/// \brief Main program of the GdNCap example

#include "DetectorConstruction.hh"
#include "ActionInitialization.hh"

#include "G4RunManagerFactory.hh"
#include "G4SteppingVerbose.hh"
#include "G4UImanager.hh"
#include "QBBC.hh"

#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

#include "Randomize.hh"

#include "QGSP_BIC_AllHP.hh"
#include "QGSP_BIC_HP.hh"
#include "G4ParticleHPManager.hh"

using namespace GdNCap;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

int main(int argc,char** argv)
{
    // Choose the Random engine
    G4Random::setTheEngine(new CLHEP::RanecuEngine);
    auto seed = time(NULL);
    G4Random::setTheSeed(seed);
  // Detect interactive mode (if no arguments) and define UI session
  //
  G4UIExecutive* ui = nullptr;
  if ( argc == 1 ) { ui = new G4UIExecutive(argc, argv); }

  // Optionally: choose a different Random engine...
  // G4Random::setTheEngine(new CLHEP::MTwistEngine);

  //use G4SteppingVerboseWithUnits
  G4int precision = 4;
  G4SteppingVerbose::UseBestUnit(precision);

  // Construct the default run manager
  //
  auto* runManager =
    G4RunManagerFactory::CreateRunManager(G4RunManagerType::Default);

  // Set mandatory initialization classes
  //
  // Detector construction
  runManager->SetUserInitialization(new DetectorConstruction());

  // Physics list
  //G4VModularPhysicsList* physicsList = new QGSP_BIC_HP;
  G4VModularPhysicsList* physicsList = new QGSP_BIC_AllHP;
  //G4VModularPhysicsList* physicsList = new QBBC;
  physicsList->SetVerboseLevel(2);
  runManager->SetUserInitialization(physicsList);

  // User action initialization
  runManager->SetUserInitialization(new ActionInitialization());

  // Replaced HP environmental variables with C++ calls
  //G4ParticleHPManager::GetInstance()->SetSkipMissingIsotopes(false);
  G4ParticleHPManager::GetInstance()->SetDoNotAdjustFinalState(true);
  G4ParticleHPManager::GetInstance()->SetUseOnlyPhotoEvaporation(true);
  //G4ParticleHPManager::GetInstance()->SetNeglectDoppler(false);
  //G4ParticleHPManager::GetInstance()->SetProduceFissionFragments(false);
  //G4ParticleHPManager::GetInstance()->SetUseWendtFissionModel(false);
  //G4ParticleHPManager::GetInstance()->SetUseNRESP71Model(false);

  // Initialize visualization
  //
  G4VisManager* visManager = new G4VisExecutive;
  // G4VisExecutive can take a verbosity argument - see /vis/verbose guidance.
  // G4VisManager* visManager = new G4VisExecutive("Quiet");
  visManager->Initialize();

  // Get the pointer to the User Interface manager
  G4UImanager* UImanager = G4UImanager::GetUIpointer();

  // Process macro or start UI session
  //
  if ( ! ui ) {
    // batch mode
    G4String command = "/control/execute ";
    G4String fileName = argv[1];
    UImanager->ApplyCommand(command+fileName);
  }
  else {
    //runManager->SetNumberOfThreads(1);
    // interactive mode
    //UImanager->ApplyCommand("/process/had/particle_hp/do_not_adjust_final_state true");
    //UImanager->ApplyCommand("/process/had/particle_hp/skip_missing_isotopes true");
    UImanager->ApplyCommand("/run/verbose 2");
    //UImanager->ApplyCommand("/event/verbose 2");
    //UImanager->ApplyCommand("/tracking/verbose 2");
    UImanager->ApplyCommand("/control/execute init_vis.mac");
    ui->SessionStart();
    delete ui;
  }

  // Job termination
  // Free the store: user actions, physics_list and detector_description are
  // owned and deleted by the run manager, so they should not be deleted
  // in the main() program !

  delete visManager;
  delete runManager;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....
