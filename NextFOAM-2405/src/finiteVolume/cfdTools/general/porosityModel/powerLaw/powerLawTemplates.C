/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2012-2016 OpenFOAM Foundation
    Copyright (C) 2018 OpenCFD Ltd.
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

\*---------------------------------------------------------------------------*/

#include "volFields.H"

// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

template<class RhoFieldType>
void Foam::porosityModels::powerLaw::apply
(
    scalarField& Udiag,
    const scalarField& V,
    const RhoFieldType& rho,
    const vectorField& U
) const
{
    const scalar C0 = C0_;
    const scalar C1m1b2 = (C1_ - 1.0)/2.0;

    if (mesh_.foundObject<volVectorField>("porousGradp"))
    {
        volVectorField& porousGradp
        (
            const_cast<volVectorField&>
            (
                mesh_.lookupObject<volVectorField>("porousGradp")
            )
        );

        for (const label zonei : cellZoneIDs_)
        {
            const labelList& cells = mesh_.cellZones()[zonei];

            for (const label celli : cells)
            {
                scalar sourceCoeff(rho[celli]*C0*pow(magSqr(U[celli]), C1m1b2));

                Udiag[celli] += V[celli]*sourceCoeff;

                //porousGradp.primitiveFieldRef()[celli] = -sourceCoeff*U[celli];
            }
        }

        //porousGradp.correctBoundaryConditions();
    }
    else
    {
        for (const label zonei : cellZoneIDs_)
        {
            const labelList& cells = mesh_.cellZones()[zonei];

            for (const label celli : cells)
            {
                Udiag[celli] +=
                    V[celli]*rho[celli]*C0*pow(magSqr(U[celli]), C1m1b2);
            }
        }
    }
}


template<class RhoFieldType>
void Foam::porosityModels::powerLaw::apply
(
    tensorField& AU,
    const RhoFieldType& rho,
    const vectorField& U
) const
{
    const scalar C0 = C0_;
    const scalar C1m1b2 = (C1_ - 1.0)/2.0;

    if (mesh_.foundObject<volVectorField>("porousGradp"))
    {
        volVectorField& porousGradp
        (
            const_cast<volVectorField&>
            (
                mesh_.lookupObject<volVectorField>("porousGradp")
            )
        );

        for (const label zonei : cellZoneIDs_)
        {
            const labelList& cells = mesh_.cellZones()[zonei];

            for (const label celli : cells)
            {
                scalar sourceCoeff(rho[celli]*C0*pow(magSqr(U[celli]), C1m1b2));

                AU[celli] = AU[celli] + I*sourceCoeff;

                porousGradp.primitiveFieldRef()[celli] = -sourceCoeff*U[celli];
            }
        }

        porousGradp.correctBoundaryConditions();
    }
    else
    {
        for (const label zonei : cellZoneIDs_)
        {
            const labelList& cells = mesh_.cellZones()[zonei];

            for (const label celli : cells)
            {
                AU[celli] =
                    AU[celli] + I*(rho[celli]*C0*pow(magSqr(U[celli]), C1m1b2));
            }
        }
    }
}


// ************************************************************************* //
