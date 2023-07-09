static MagickRealType ApplyEvaluateOperator(RandomInfo *random_info,
  const Quantum pixel,const MagickEvaluateOperator op,
  const MagickRealType value)
{
  MagickRealType
    result;

  result=0.0;
  switch (op)
  {
    case UndefinedEvaluateOperator:
      break;
    case AbsEvaluateOperator:
    {
      result=(MagickRealType) fabs((double) (pixel+value));
      break;
    }
    case AddEvaluateOperator:
    {
      result=(MagickRealType) (pixel+value);
      break;
    }
    case AddModulusEvaluateOperator:
    {
      /*
        This returns a 'floored modulus' of the addition which is a
        positive result.  It differs from  % or fmod() which returns a
        'truncated modulus' result, where floor() is replaced by trunc()
        and could return a negative result (which is clipped).
      */
      result=pixel+value;
      result-=(QuantumRange+1.0)*floor((double) result/(QuantumRange+1.0));
      break;
    }
    case AndEvaluateOperator:
    {
      result=(MagickRealType) ((size_t) pixel & (size_t) (value+0.5));
      break;
    }
    case CosineEvaluateOperator:
    {
      result=(MagickRealType) (QuantumRange*(0.5*cos((double) (2.0*MagickPI*
        QuantumScale*pixel*value))+0.5));
      break;
    }
    case DivideEvaluateOperator:
    {
      result=pixel/(value == 0.0 ? 1.0 : value);
      break;
    }
    case ExponentialEvaluateOperator:
    {
      result=(MagickRealType) (QuantumRange*exp((double) (value*QuantumScale*
        pixel)));
      break;
    }
    case GaussianNoiseEvaluateOperator:
    {
      result=(MagickRealType) GenerateDifferentialNoise(random_info,pixel,
        GaussianNoise,value);
      break;
    }
    case ImpulseNoiseEvaluateOperator:
    {
      result=(MagickRealType) GenerateDifferentialNoise(random_info,pixel,
        ImpulseNoise,value);
      break;
    }
    case LaplacianNoiseEvaluateOperator:
    {
      result=(MagickRealType) GenerateDifferentialNoise(random_info,pixel,
        LaplacianNoise,value);
      break;
    }
    case LeftShiftEvaluateOperator:
    {
      result=(MagickRealType) ((size_t) pixel << (size_t) (value+0.5));
      break;
    }
    case LogEvaluateOperator:
    {
      if ((QuantumScale*pixel) >= MagickEpsilon)
        result=(MagickRealType) (QuantumRange*log((double) (QuantumScale*value*
          pixel+1.0))/log((double) (value+1.0)));
      break;
    }
    case MaxEvaluateOperator:
    {
      result=(MagickRealType) EvaluateMax((double) pixel,value);
      break;
    }
    case MeanEvaluateOperator:
    {
      result=(MagickRealType) (pixel+value);
      break;
    }
    case MedianEvaluateOperator:
    {
      result=(MagickRealType) (pixel+value);
      break;
    }
    case MinEvaluateOperator:
    {
      result=(MagickRealType) MagickMin((double) pixel,value);
      break;
    }
    case MultiplicativeNoiseEvaluateOperator:
    {
      result=(MagickRealType) GenerateDifferentialNoise(random_info,pixel,
        MultiplicativeGaussianNoise,value);
      break;
    }
    case MultiplyEvaluateOperator:
    {
      result=(MagickRealType) (value*pixel);
      break;
    }
    case OrEvaluateOperator:
    {
      result=(MagickRealType) ((size_t) pixel | (size_t) (value+0.5));
      break;
    }
    case PoissonNoiseEvaluateOperator:
    {
      result=(MagickRealType) GenerateDifferentialNoise(random_info,pixel,
        PoissonNoise,value);
      break;
    }
    case PowEvaluateOperator:
    {
      result=(MagickRealType) (QuantumRange*pow((double) (QuantumScale*pixel),
        (double) value));
      break;
    }
    case RightShiftEvaluateOperator:
    {
      result=(MagickRealType) ((size_t) pixel >> (size_t) (value+0.5));
      break;
    }
    case RootMeanSquareEvaluateOperator:
    {
      result=(MagickRealType) (pixel*pixel+value);
      break;
    }
    case SetEvaluateOperator:
    {
      result=value;
      break;
    }
    case SineEvaluateOperator:
    {
      result=(MagickRealType) (QuantumRange*(0.5*sin((double) (2.0*MagickPI*
        QuantumScale*pixel*value))+0.5));
      break;
    }
    case SubtractEvaluateOperator:
    {
      result=(MagickRealType) (pixel-value);
      break;
    }
    case SumEvaluateOperator:
    {
      result=(MagickRealType) (pixel+value);
      break;
    }
    case ThresholdEvaluateOperator:
    {
      result=(MagickRealType) (((MagickRealType) pixel <= value) ? 0 :
        QuantumRange);
      break;
    }
    case ThresholdBlackEvaluateOperator:
    {
      result=(MagickRealType) (((MagickRealType) pixel <= value) ? 0 : pixel);
      break;
    }
    case ThresholdWhiteEvaluateOperator:
    {
      result=(MagickRealType) (((MagickRealType) pixel > value) ? QuantumRange :
        pixel);
      break;
    }
    case UniformNoiseEvaluateOperator:
    {
      result=(MagickRealType) GenerateDifferentialNoise(random_info,pixel,
        UniformNoise,value);
      break;
    }
    case XorEvaluateOperator:
    {
      result=(MagickRealType) ((size_t) pixel ^ (size_t) (value+0.5));
      break;
    }
  }
  return(result);
}