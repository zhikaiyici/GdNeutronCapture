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

// Class template for accumulables handled by Geant4 analysis
//
// Author: Ivana Hrivnacova, 07/09/2015  (ivana@ipno.in2p3.fr)

#ifndef GdNCapAccumulable_h
#define GdNCapAccumulable_h 1

#include "G4VAccumulable.hh"

#include "globals.hh"

#include <list>
#include <vector>

namespace GdNCap
{

    class Accumulable : public G4VAccumulable
    {
    public:
        Accumulable();
        ~Accumulable();

        // Methods
        void Merge(const G4VAccumulable& other) final;
        void Reset() final;

        // Get methods
        inline std::list<std::vector<std::pair<G4double, G4String>>> GetSecondariesList() const { return secondariesList; }
        inline std::list<G4double> GetSecondariesEnergy() const { return secondariesEnergy; }
        void PushSecondariesList(std::vector<std::pair<G4double, G4String>> fSecondaries);
        void PushSecondariesEnergy(G4double fSecE);

    private:
        // Data members
        std::list<std::vector<std::pair<G4double, G4String>>> secondariesList;
        std::list<G4double> secondariesEnergy;
    };

}

#endif
