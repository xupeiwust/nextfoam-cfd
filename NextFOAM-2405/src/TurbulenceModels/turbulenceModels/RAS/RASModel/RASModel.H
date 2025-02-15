/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2013-2017 OpenFOAM Foundation
    Copyright (C) 2021-2022 OpenCFD Ltd.
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

Class
    Foam::RASModel

Description
    Templated abstract base class for RAS turbulence models

Author
    Modified JaeHeung Gill, NEXTfoam Co.,Ltd.

SourceFiles
    RASModel.C

\*---------------------------------------------------------------------------*/

#ifndef Foam_RASModel_H
#define Foam_RASModel_H

#include "TurbulenceModel.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                        Class RASModelBase Declaration
\*---------------------------------------------------------------------------*/

class RASModelBase
{
public:

    //- Runtime type information
    ClassName("RASModelBase");


    // Constructors

        //- Constructor
        RASModelBase() noexcept = default;


    //- Destructor
    virtual ~RASModelBase() = default;
};


/*---------------------------------------------------------------------------*\
                           Class RASModel Declaration
\*---------------------------------------------------------------------------*/

template<class BasicTurbulenceModel>
class RASModel
:
    public RASModelBase,
    public BasicTurbulenceModel
{
protected:

    // Protected Data

        //- RAS coefficients dictionary
        dictionary RASDict_;

        //- Turbulence on/off flag
        Switch turbulence_;

        //- Flag to print the model coeffs at run-time
        Switch printCoeffs_;

        //- Model coefficients dictionary
        dictionary coeffDict_;

        //- Lower limit of k
        dimensionedScalar kMin_;

        //- Lower limit of epsilon
        dimensionedScalar epsilonMin_;

        //- Lower limit for omega
        dimensionedScalar omegaMin_;

        //- Lower limit for viscosity ratio  // by Gill
        dimensionedScalar viscosityRatioMin_;

        //- Upper limit for viscosity ratio  // by Gill
        dimensionedScalar viscosityRatioMax_;


    // Protected Member Functions

        //- Print model coefficients
        virtual void printCoeffs(const word& type);

        //- No copy construct
        RASModel(const RASModel&) = delete;

        //- No copy assignment
        void operator=(const RASModel&) = delete;


public:

    typedef typename BasicTurbulenceModel::alphaField alphaField;
    typedef typename BasicTurbulenceModel::rhoField rhoField;
    typedef typename BasicTurbulenceModel::transportModel transportModel;


    //- Runtime type information
    TypeName("RAS");


    // Declare run-time constructor selection table

        declareRunTimeSelectionTable
        (
            autoPtr,
            RASModel,
            dictionary,
            (
                const alphaField& alpha,
                const rhoField& rho,
                const volVectorField& U,
                const surfaceScalarField& alphaRhoPhi,
                const surfaceScalarField& phi,
                const transportModel& transport,
                const word& propertiesName
            ),
            (alpha, rho, U, alphaRhoPhi, phi, transport, propertiesName)
        );


    // Constructors

        //- Construct from components
        RASModel
        (
            const word& type,
            const alphaField& alpha,
            const rhoField& rho,
            const volVectorField& U,
            const surfaceScalarField& alphaRhoPhi,
            const surfaceScalarField& phi,
            const transportModel& transport,
            const word& propertiesName
        );


    // Selectors

        //- Return a reference to the selected RAS model
        static autoPtr<RASModel> New
        (
            const alphaField& alpha,
            const rhoField& rho,
            const volVectorField& U,
            const surfaceScalarField& alphaRhoPhi,
            const surfaceScalarField& phi,
            const transportModel& transport,
            const word& propertiesName = turbulenceModel::propertiesName
        );


    //- Destructor
    virtual ~RASModel() = default;


    // Member Functions

        //- Read model coefficients if they have changed
        virtual bool read();


        // Access

            //- Return the lower allowable limit for k (default: SMALL)
            const dimensionedScalar& kMin() const noexcept
            {
                return kMin_;
            }

            //- Return the lower allowable limit for epsilon (default: SMALL)
            const dimensionedScalar& epsilonMin() const noexcept
            {
                return epsilonMin_;
            }

            //- Return the lower allowable limit for omega (default: SMALL)
            const dimensionedScalar& omegaMin() const noexcept
            {
                return omegaMin_;
            }

            //- Return the lower allowable limit for viscosity ratio 
            // (default: 0.0)
            const dimensionedScalar& viscosityRatioMin() const // by Gill
            {
                return viscosityRatioMin_;
            }

            //- Return the upper allowable limit for viscosity ratio 
            // (default: 1E5)
            const dimensionedScalar& viscosityRatioMax() const // by Gill
            {
                return viscosityRatioMax_;
            }

            //- Allow kMin to be changed
            dimensionedScalar& kMin() noexcept
            {
                return kMin_;
            }

            //- Allow epsilonMin to be changed
            dimensionedScalar& epsilonMin() noexcept
            {
                return epsilonMin_;
            }

            //- Allow omegaMin to be changed
            dimensionedScalar& omegaMin() noexcept
            {
                return omegaMin_;
            }

            //- Allow viscosityRatioMin to be changed
            dimensionedScalar& viscosityRatioMin() // by Gill
            {
                return viscosityRatioMin_;
            }

            //- Allow viscosityRatioMax to be changed
            dimensionedScalar& viscosityRatioMax() // by Gill
            {
                return viscosityRatioMax_;
            }

            //- Const access to the coefficients dictionary
            virtual const dictionary& coeffDict() const
            {
                return coeffDict_;
            }


        //- Return the effective viscosity
        virtual tmp<volScalarField> nuEff() const
        {
            return tmp<volScalarField>
            (
                new volScalarField
                (
                    IOobject::groupName("nuEff", this->alphaRhoPhi_.group()),
                    this->nut() + this->nu()
                )
            );
        }

        //- Return the effective viscosity on patch
        virtual tmp<scalarField> nuEff(const label patchi) const
        {
            return this->nut(patchi) + this->nu(patchi);
        }

        //- Return the turbulence kinetic energy dissipation rate
        virtual tmp<volScalarField> epsilon() const;

        //- Return the specific dissipation rate
        virtual tmp<volScalarField> omega() const;

        //- Solve the turbulence equations and correct the turbulence viscosity
        virtual void correct();
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#ifdef NoRepository
    #include "RASModel.C"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
