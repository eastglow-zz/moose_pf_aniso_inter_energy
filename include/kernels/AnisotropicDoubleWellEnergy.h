//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#ifndef ANISOTROPICDOUBLEWELLENERGY_H
#define ANISOTROPICDOUBLEWELLENERGY_H

#include "Kernel.h"
#include "JvarMapInterface.h"
#include "DerivativeMaterialInterface.h"

class AnisotropicDoubleWellEnergy;

template <>
InputParameters validParams<AnisotropicDoubleWellEnergy>();

class AnisotropicDoubleWellEnergy : public DerivativeMaterialInterface<JvarMapKernelInterface<Kernel>>
{
public:
  AnisotropicDoubleWellEnergy(const InputParameters & parameters);
  virtual void initialSetup();

protected:
  RealGradient get_dfbulk_darg(unsigned int qp);
  RealGradient get_d2fbulk_darg2(unsigned int cvar, unsigned int qp);
  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();

  const unsigned int _nvar;
  /// Phase-field mobility (assumed to be constant)
  const MaterialProperty<Real> & _L;
  /// Anisotropic double well type function; 0th order derivative from Material data
  const MaterialProperty<Real> & _fbulk;
  /// Anisotropic double well type function; 1st order derivative
  const MaterialProperty<Real> & _dfbulk_dvar;
  /// Anisotropic double well type function; 2nd order derivative
  const MaterialProperty<Real> & _d2fbulk_dvar2;
  /// Threshold value of the magnitude of grad_aeta
  const Real _gradmag_threshold;
  /// Anisotropic ouble well function; 1st order derivatives over the gradient components from Material data
  std::vector<const MaterialProperty<Real> *> _dfbulk_darg;
  /// Anisotropic double well function; 2nd order derivatives over the gradient components from Material data
  std::vector<std::vector<const MaterialProperty<Real> *>> _d2fbulk_darg2;
};

#endif // ANISOTROPICDOUBLEWELLENERGY_H
