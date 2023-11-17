#include "Accumulable.hh"

namespace GdNCap
{
	Accumulable::Accumulable() :G4VAccumulable() {}
	Accumulable::~Accumulable() 
	{
		secondariesList.clear();
	}

	void Accumulable::Merge(const G4VAccumulable& other)
	{
		const Accumulable& otherMatrix = static_cast<const Accumulable&>(other);
		std::list<std::vector<std::pair<G4double, G4String>>> secListTemp = otherMatrix.secondariesList;
		std::list<G4double> secEnergyTemp = otherMatrix.secondariesEnergy;
		secondariesList.merge(secListTemp);
		secondariesEnergy.merge(secEnergyTemp);
	}

	void Accumulable::Reset()
	{
		secondariesList.clear();
	}
	
	void Accumulable::PushSecondariesList(std::vector<std::pair<G4double, G4String>> fSecondaries)
	{
		secondariesList.push_back(fSecondaries);
	}
	void Accumulable::PushSecondariesEnergy(G4double fSecE)
	{
		secondariesEnergy.push_back(fSecE);
	}
}