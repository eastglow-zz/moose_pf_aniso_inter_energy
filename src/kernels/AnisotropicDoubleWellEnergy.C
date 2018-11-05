//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html



#include "AnisotropicDoubleWellEnergy.h"

registerMooseObject("PhaseFieldApp", AnisotropicDoubleWellEnergy);

template <>
InputParameters
validParams<AnisotropicDoubleWellEnergy>()
{
  InputParameters params = validParams<Kernel>();
  params.addClassDescription("Ganaralized AllenCahn kernel; calculates a general form of L * (d/daeta - div((d/dgrad_aeta)) fbulk(aeta, grad_aeta)");
  params.addParam<MaterialPropertyName>("mob_name","L","The mobility used with the kernel, assumed as a constant");
  params.addParam<MaterialPropertyName>("fbulk_name","fbulk","Double-well type function in terms of aeta and the components of grad_aeta which can be provided by DerivativeParsedMaterial with derivative_order 2");
  params.addParam<Real>("gradmag_threshold",-1,"Threshold value to turn on anisotropy term calculations; grad_mag > thres ? anisotropic calc. : isotropic calc.");
  params.addCoupledVar("gradient_component_names","Name vector of gradient components of aeta, arguments of the kappa function, in x y z order, e.g.) in 2D, dpx dpy, in 3D, dpx dpy dpz, where dpx, dpy, and dpz are defined as AuxVariables(FIRST order, MONOMIAL family) and calculated by VariableGradientComponent AuxKernel (execute_on = LINEAR)");
  return params;
}

AnisotropicDoubleWellEnergy::AnisotropicDoubleWellEnergy(const InputParameters & parameters)
  : DerivativeMaterialInterface<JvarMapKernelInterface<Kernel>>(parameters),
  _nvar(_coupled_moose_vars.size()),
  _L(getMaterialProperty<Real>("mob_name")),
  _fbulk(getMaterialProperty<Real>("fbulk_name")),
  _dfbulk_dvar(getMaterialPropertyDerivative<Real>("fbulk_name", _var.name())),
  _d2fbulk_dvar2(getMaterialPropertyDerivative<Real>("fbulk_name", _var.name(), _var.name())),
  _gradmag_threshold(getParam<Real>("gradmag_threshold")),
  _dfbulk_darg(_nvar),
  _d2fbulk_darg2(_nvar)
{
  /// Get derivative data
  for (unsigned int i = 0; i < _nvar; ++i)
  {
    MooseVariable * ivar = _coupled_standard_moose_vars[i];
    const VariableName iname = ivar->name();
    if (iname == _var.name())
      paramError("gradient_component_names",\
                 "The kernel variable should not be specified in the coupled `gradient_component_names` parameter.");

    /// The 1st derivatives
    _dfbulk_darg[i] = &getMaterialPropertyDerivative<Real>("fbulk_name", iname);

    /// The 2nd derivatives
    _d2fbulk_darg2[i].resize(_nvar);
    for (unsigned int j = 0; j < _nvar; ++j)
    {
      const VariableName jname = _coupled_moose_vars[j]->name();
      if (jname == _var.name())
        paramError("gradient_component_names",\
                   "The kernel variable should not be specified in the coupled `gradient_component_names` parameter.");
      _d2fbulk_darg2[i][j] = &getMaterialPropertyDerivative<Real>("fbulk_name", iname, jname);
    }
  }
}

void
AnisotropicDoubleWellEnergy::initialSetup()
{
  //validateCoupling<Real>("w_name");
  validateNonlinearCoupling<Real>("fbulk_name");
  validateDerivativeMaterialPropertyBase<Real>("fbulk_name");
}

RealGradient
AnisotropicDoubleWellEnergy::get_dfbulk_darg(unsigned int qp) // This function must be called in computeQp* functions
{
  RealGradient v0(0.0, 0.0, 0.0);
  switch (_nvar) {
    case 1:
      {
        RealGradient v1((*_dfbulk_darg[0])[qp], 0.0, 0.0);
        return v1;
      }
      break;
    case 2:
      {
        RealGradient v2((*_dfbulk_darg[0])[qp], (*_dfbulk_darg[1])[qp], 0.0);
        return v2;
      }
      break;
    case 3:
      {
        RealGradient v3((*_dfbulk_darg[0])[qp], (*_dfbulk_darg[1])[qp], (*_dfbulk_darg[2])[qp]);
        return v3;
      }
      break;
    default:
      return v0;
  }
}

RealGradient
AnisotropicDoubleWellEnergy::get_d2fbulk_darg2(unsigned int i, unsigned int qp)  // This function must be called in computeQp* functions
{
  RealGradient v0(0.0, 0.0, 0.0);
  switch (_nvar) {
    case 1:
      {
        RealGradient v1((*_d2fbulk_darg2[i][0])[qp], 0.0, 0.0);
        return v1;
      }
      break;
    case 2:
      {
        RealGradient v2((*_d2fbulk_darg2[i][0])[qp], (*_d2fbulk_darg2[i][1])[qp], 0.0);
        return v2;
        break;
      }
      break;
    case 3:
      {
        RealGradient v3((*_d2fbulk_darg2[i][0])[qp], (*_d2fbulk_darg2[i][1])[qp], (*_d2fbulk_darg2[i][2])[qp]);
        return v3;
      }
      break;
    default:
      return v0;
  }
}

Real
AnisotropicDoubleWellEnergy::computeQpResidual()
{
  Real grad_u_sq = _grad_u[_qp] * _grad_u[_qp];
  if (_nvar > 0 && sqrt(grad_u_sq) > _gradmag_threshold)
  {
    RealGradient dfbulk_dgradaeta = get_dfbulk_darg(_qp);
    Real dfbulk_dgradaeta_dot_grad_test = dfbulk_dgradaeta * _grad_test[_i][_qp];
    return _L[_qp] * (_dfbulk_dvar[_qp] * _test[_i][_qp]\
                      + dfbulk_dgradaeta_dot_grad_test);
  }else{
    return _L[_qp] * _dfbulk_dvar[_qp] * _test[_i][_qp];
  }
}

Real
AnisotropicDoubleWellEnergy::computeQpJacobian()
{
  Real grad_u_sq = _grad_u[_qp] * _grad_u[_qp];
  if (_nvar > 0 && sqrt(grad_u_sq) > _gradmag_threshold)
  {
    Real d2fbulk_dgradaeta2_dot_grad_phi_dot_grad_test = 0.0;
    for (unsigned int i = 0; i < _nvar; ++i)
    {
      for (unsigned int j = 0; j < _nvar; ++j)
      {
        d2fbulk_dgradaeta2_dot_grad_phi_dot_grad_test += \
          _grad_test[_i][_qp](i) * (*_d2fbulk_darg2[i][j])[_qp] * _grad_phi[_j][_qp](j);
      }
    }
    return _L[_qp] * ( _d2fbulk_dvar2[_qp] * _phi[_j][_qp] * _test[_i][_qp]\
                     + d2fbulk_dgradaeta2_dot_grad_phi_dot_grad_test);
  }else{
    return _L[_qp] * _d2fbulk_dvar2[_qp] * _phi[_j][_qp] * _test[_i][_qp];
  }
}
