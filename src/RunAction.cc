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
/// \file GdNCap/src/RunAction.cc
/// \brief Implementation of the GdNCap::RunAction class

#include "RunAction.hh"
#include "PrimaryGeneratorAction.hh"
#include "DetectorConstruction.hh"
// #include "Run.hh"

#include "G4RunManager.hh"
#include "G4Run.hh"
#include "G4AccumulableManager.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

#include "G4HadronicInteraction.hh"
#include "G4HadronicInteractionRegistry.hh"
#include "G4INCLXXInterface.hh"
#include "G4AblaInterface.hh"
#include "G4INCLXXInterfaceStore.hh"

#include <fstream>

namespace GdNCap
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

RunAction::RunAction()
{
  // add new units for dose
  //
  const G4double milligray = 1.e-3*gray;
  const G4double microgray = 1.e-6*gray;
  const G4double nanogray  = 1.e-9*gray;
  const G4double picogray  = 1.e-12*gray;

  new G4UnitDefinition("milligray", "milliGy" , "Dose", milligray);
  new G4UnitDefinition("microgray", "microGy" , "Dose", microgray);
  new G4UnitDefinition("nanogray" , "nanoGy"  , "Dose", nanogray);
  new G4UnitDefinition("picogray" , "picoGy"  , "Dose", picogray);

  fSecondaries = new Accumulable();

  // Register accumulable to the accumulable manager
  G4AccumulableManager* accumulableManager = G4AccumulableManager::Instance();
  accumulableManager->RegisterAccumulable(fEdep);
  accumulableManager->RegisterAccumulable(fEdep2);
  accumulableManager->RegisterAccumulable(fSecondaries);
  //G4RunManager::GetRunManager()->SetPrintProgress(10);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

RunAction::~RunAction()
{
    delete fSecondaries;
}

void RunAction::BeginOfRunAction(const G4Run*)
{
    // Get hold of pointers to the INCL++ model interfaces
    std::vector<G4HadronicInteraction*> interactions = 
        G4HadronicInteractionRegistry::Instance()->FindAllModels(G4INCLXXInterfaceStore::GetInstance()->getINCLXXVersionName());
    for (std::vector<G4HadronicInteraction*>::const_iterator iInter = interactions.begin(), e = interactions.end(); iInter != e; ++iInter)
    {
        G4INCLXXInterface* theINCLInterface = static_cast<G4INCLXXInterface*>(*iInter);
        if (theINCLInterface)
        {
            // Instantiate the ABLA model
            G4HadronicInteraction* interaction = G4HadronicInteractionRegistry::Instance()->FindModel("ABLA");
            G4AblaInterface* theAblaInterface = static_cast<G4AblaInterface*>(interaction);
            if (!theAblaInterface)
                theAblaInterface = new G4AblaInterface;
            // Couple INCL++ to ABLA
            G4cout << "Coupling INCLXX to ABLA" << G4endl;
            theINCLInterface->SetDeExcitation(theAblaInterface);
        }
    }
  // inform the runManager to save random number seed
  G4RunManager::GetRunManager()->SetRandomNumberStore(false);

  // reset accumulables to their initial values
  G4AccumulableManager* accumulableManager = G4AccumulableManager::Instance();
  accumulableManager->Reset();

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RunAction::EndOfRunAction(const G4Run* run)
{
  G4int nofEvents = run->GetNumberOfEvent();
  if (nofEvents == 0) return;

  // Merge accumulables
  G4AccumulableManager* accumulableManager = G4AccumulableManager::Instance();
  accumulableManager->Merge();

  // Compute dose = total energy deposit in a run and its variance
  //
  G4double edep  = fEdep.GetValue();
  G4double edep2 = fEdep2.GetValue();
  auto secondariesList = fSecondaries->GetSecondariesList();
  auto secondariesEnergy = fSecondaries->GetSecondariesEnergy();

  G4double rms = edep2 - edep*edep/nofEvents;
  if (rms > 0.) rms = std::sqrt(rms); else rms = 0.;

  const auto detConstruction = static_cast<const DetectorConstruction*>(
    G4RunManager::GetRunManager()->GetUserDetectorConstruction());
  G4double mass = detConstruction->GetScoringVolume()->GetMass();
  G4double dose = edep/mass;
  G4double rmsDose = rms/mass;

  // Run conditions
  //  note: There is no primary generator action object for "master"
  //        run manager for multi-threaded mode.
  const auto generatorAction = static_cast<const PrimaryGeneratorAction*>(
    G4RunManager::GetRunManager()->GetUserPrimaryGeneratorAction());
  G4String runCondition;
  if (generatorAction)
  {
    const G4ParticleGun* particleGun = generatorAction->GetParticleGun();
    runCondition += particleGun->GetParticleDefinition()->GetParticleName();
    runCondition += " of ";
    G4double particleEnergy = particleGun->GetParticleEnergy();
    runCondition += G4BestUnit(particleEnergy,"Energy");
  }

  // Print
  //
  if (IsMaster()) {
    G4cout
     << G4endl
     << "--------------------End of Global Run-----------------------";

    G4String totalEnergyName = "SecondaryTotalEnergy.txt";
    G4String energyName = "SecondaryEnergy.txt";
    G4String nameName = "SecondaryName.txt";
    if (!secondariesList.empty())
    {
        std::ofstream totalEnergyFile;
        std::ofstream energyFile;
        std::ofstream nameFile;
        totalEnergyFile.open(totalEnergyName, std::ios_base::out);
        energyFile.open(energyName, std::ios_base::out);
        nameFile.open(nameName, std::ios_base::out);
        for (auto itr = secondariesEnergy.begin(); itr != secondariesEnergy.end(); ++itr)
        {
            if (*itr > 0)
            {
                totalEnergyFile << *itr << G4endl;
            }            
        }
        totalEnergyFile.close();
        for (auto itrVec = secondariesList.begin(); itrVec != secondariesList.end(); ++itrVec)
        {
            if (itrVec->size() > 0)
            {
                for (auto itr = itrVec->begin(); itr != itrVec->end(); ++itr)
                {
                    energyFile << itr->first << " ";
                    nameFile << itr->second << " ";
                }
                energyFile << G4endl;
                nameFile << G4endl;
            }
        }
        energyFile.close();
        nameFile.close();
    }
  }
  else {
    G4cout
     << G4endl
     << "--------------------End of Local Run------------------------";
  }

  G4cout
     << G4endl
     << " The run consists of " << nofEvents << " "<< runCondition
     << G4endl
     << " Cumulated dose per run, in scoring volume : "
     << G4BestUnit(dose,"Dose") << " rms = " << G4BestUnit(rmsDose,"Dose")
     << G4endl
     << "------------------------------------------------------------"
     << G4endl
     << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RunAction::AddEdep(G4double edep)
{
  fEdep  += edep;
  fEdep2 += edep*edep;
}

void RunAction::PushSecondariesList(std::vector<std::pair<G4double, G4String>> fSecL)
{
    fSecondaries->PushSecondariesList(fSecL);
}

void RunAction::PushSecondariesEnergy(G4double fSecE)
{
    fSecondaries->PushSecondariesEnergy(fSecE);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}
