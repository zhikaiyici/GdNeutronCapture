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
/// \file GdNCap/src/DetectorConstruction.cc
/// \brief Implementation of the GdNCap::DetectorConstruction class

#include "DetectorConstruction.hh"

#include "G4RunManager.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Cons.hh"
#include "G4Orb.hh"
#include "G4Sphere.hh"
#include "G4Trd.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"

#include "G4Isotope.hh"

#include "G4VisAttributes.hh"

namespace GdNCap
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  // Get nist material manager
  G4NistManager* nist = G4NistManager::Instance();

  G4Isotope* Gd155 = new G4Isotope("Gd155", 64, 155, 154.9226294 * g / mole);
  G4Element* elGd155 = new G4Element("enrichedGd155", "enGd155", 1);
  elGd155->AddIsotope(Gd155, 100. * perCent);
  G4Material* myGd155 = new G4Material("myGd155", 7.9 * g / cm3, 1);
  myGd155->AddElement(elGd155, 100. * perCent);

  G4Isotope* Gd157 = new G4Isotope("Gd157", 64, 157, 156.9239674 * g / mole);
  G4Element* elGd157 = new G4Element("enrichedGd157", "enGd157", 1);
  elGd157->AddIsotope(Gd157, 100. * perCent);
  G4Material* myGd157 = new G4Material("myGd157", 7.9 * g / cm3, 1);
  myGd157->AddElement(elGd157, 100. * perCent);

  G4Isotope* H1 = new G4Isotope("H1", 1, 1, 1.007825 * g / mole);
  G4Element* elH1 = new G4Element("enrichedH1", "enH1", 1);
  elH1->AddIsotope(H1, 100. * perCent);
  G4Material* myH1 = new G4Material("myH1", 1.0 * g / cm3, 1);
  myH1->AddElement(elH1, 100. * perCent);

  G4Isotope* C12 = new G4Isotope("C12", 6, 12, 12.0 * g / mole);
  G4Element* elC12 = new G4Element("enrichedC12", "enC12", 1);
  elC12->AddIsotope(C12, 100. * perCent);
  G4Material* myC12 = new G4Material("myC12", 2.267 * g / cm3, 1);
  myC12->AddElement(elC12, 100. * perCent);

  G4Material* natC = nist->FindOrBuildMaterial("G4_C");

  // Envelope parameters
  //
  G4double env_sizeXY = 5. * cm, env_sizeZ = 1. * cm;
  G4Material* env_mat = nist->FindOrBuildMaterial("myGd157");

  // Option to switch on/off checking of volumes overlaps
  //
  G4bool checkOverlaps = true;

  //
  // World
  //
  G4double world_sizeXY = 1.2*env_sizeXY;
  G4double world_sizeZ  = 1.2*env_sizeZ;
  G4Material* world_mat = nist->FindOrBuildMaterial("G4_AIR");

  auto solidWorld = new G4Box("World",                           // its name
    0.5 * world_sizeXY, 0.5 * world_sizeXY, 0.5 * world_sizeZ);  // its size

  auto logicWorld = new G4LogicalVolume(solidWorld,  // its solid
    world_mat,                                       // its material
    "World");                                        // its name

  G4VisAttributes* vis = new G4VisAttributes(false);
  logicWorld->SetVisAttributes(vis);

  auto physWorld = new G4PVPlacement(nullptr,  // no rotation
    G4ThreeVector(),                           // at (0,0,0)
    logicWorld,                                // its logical volume
    "World",                                   // its name
    nullptr,                                   // its mother  volume
    false,                                     // no boolean operation
    0,                                         // copy number
    checkOverlaps);                            // overlaps checking

  //
  // Envelope
  //
  auto solidEnv = new G4Box("Envelope",                    // its name
    0.5 * env_sizeXY, 0.5 * env_sizeXY, 0.5 * env_sizeZ);  // its size

  auto logicEnv = new G4LogicalVolume(solidEnv,  // its solid
    env_mat,                                     // its material
    "Envelope");                                 // its name

  new G4PVPlacement(nullptr,  // no rotation
    G4ThreeVector(),          // at (0,0,0)
    logicEnv,                 // its logical volume
    "Envelope",               // its name
    logicWorld,               // its mother  volume
    false,                    // no boolean operation
    0,                        // copy number
    checkOverlaps);           // overlaps checking

  // Set Shape2 as scoring volume
  //
  fScoringVolume = logicEnv;

  G4cout << *(G4Material::GetMaterialTable()) << G4endl;
  //
  //always return the physical World
  //
  return physWorld;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}
