void Scan::CreateParser(void)
{
  ScanType type = m_pFrame->ScanTypeOf();
  //
  assert(m_pParser == NULL);
  //
  switch(type) {
  case Baseline:
    m_pParser = new(m_pEnviron) class SequentialScan(m_pFrame,this,
                                                     m_ucScanStart,m_ucScanStop,
                                                     m_ucLowBit + m_ucHiddenBits,
                                                     m_ucHighBit + m_ucHiddenBits,
                                                     false,false,false,true);
    break;
  case Sequential:
    m_pParser = new(m_pEnviron) class SequentialScan(m_pFrame,this,
                                                     m_ucScanStart,m_ucScanStop,
                                                     m_ucLowBit + m_ucHiddenBits,
                                                     m_ucHighBit + m_ucHiddenBits);
    break;
  case DifferentialSequential:
    m_pParser = new(m_pEnviron) class SequentialScan(m_pFrame,this,
                                                     m_ucScanStart,m_ucScanStop,
                                                     m_ucLowBit + m_ucHiddenBits,
                                                     m_ucHighBit + m_ucHiddenBits,true);
    break;
  case Lossless:
    m_pParser = new(m_pEnviron) class LosslessScan(m_pFrame,this,m_ucScanStart,
                                                   m_ucLowBit + m_ucHiddenBits);
    break;
  case DifferentialLossless:
    m_pParser = new(m_pEnviron) class LosslessScan(m_pFrame,this,0,
                                                   m_ucLowBit + m_ucHiddenBits,true);
    break;
  case ACLossless:
    m_pParser = new(m_pEnviron) class ACLosslessScan(m_pFrame,this,m_ucScanStart,
                                                     m_ucLowBit + m_ucHiddenBits);
    break;
  case ACDifferentialLossless:
    m_pParser = new(m_pEnviron) class ACLosslessScan(m_pFrame,this,0,
                                                     m_ucLowBit + m_ucHiddenBits,true);
    break;
  case ACSequential:
    m_pParser = new(m_pEnviron) class ACSequentialScan(m_pFrame,this,
                                                       m_ucScanStart,m_ucScanStop,
                                                       m_ucLowBit + m_ucHiddenBits,
                                                       m_ucHighBit + m_ucHiddenBits);
    break;
  case ACDifferentialSequential:
    m_pParser = new(m_pEnviron) class ACSequentialScan(m_pFrame,this,
                                                       m_ucScanStart,m_ucScanStop,
                                                       m_ucLowBit + m_ucHiddenBits,
                                                       m_ucHighBit + m_ucHiddenBits,true);
    break;
  case Progressive:
    if (m_ucHighBit == 0) { // The first scan is parsed off by the regular parser.
      m_pParser = new(m_pEnviron) class SequentialScan(m_pFrame,this,
                                                       m_ucScanStart,m_ucScanStop,
                                                       m_ucLowBit + m_ucHiddenBits,
                                                       m_ucHighBit + m_ucHiddenBits);
    } else { 
      m_pParser = new(m_pEnviron) class RefinementScan(m_pFrame,this,
                                                       m_ucScanStart,m_ucScanStop,
                                                       m_ucLowBit + m_ucHiddenBits,
                                                       m_ucHighBit + m_ucHiddenBits);
    }
    break;
  case ResidualProgressive:
    if (m_ucHighBit == 0) { 
      m_pParser = new(m_pEnviron) class SequentialScan(m_pFrame,this,
                                                       m_ucScanStart,m_ucScanStop,
                                                       m_ucLowBit + m_ucHiddenBits,
                                                       m_ucHighBit + m_ucHiddenBits,
                                                       true,true);
    } else { 
      m_pParser = new(m_pEnviron) class RefinementScan(m_pFrame,this,
                                                       m_ucScanStart,m_ucScanStop,
                                                       m_ucLowBit + m_ucHiddenBits,
                                                       m_ucHighBit + m_ucHiddenBits,
                                                       true,true);
    }
    break;
  case DifferentialProgressive:
    if (m_ucHighBit == 0) { // The first scan is parsed off by the regular parser.
      m_pParser = new(m_pEnviron) class SequentialScan(m_pFrame,this,
                                                       m_ucScanStart,m_ucScanStop,
                                                       m_ucLowBit + m_ucHiddenBits,
                                                       m_ucHighBit + m_ucHiddenBits,true);
    } else { 
      // Even though the specs do not mention this, it makes perfect sense that the
      // refinement scan is a regular refinement scan without modification.
      m_pParser = new(m_pEnviron) class RefinementScan(m_pFrame,this,
                                                       m_ucScanStart,m_ucScanStop,
                                                       m_ucLowBit + m_ucHiddenBits,
                                                       m_ucHighBit + m_ucHiddenBits,true);
    }
    break;
  case ACProgressive: 
    if (m_ucHighBit == 0) { // The first scan is parsed off by the regular parser.
      m_pParser = new(m_pEnviron) class ACSequentialScan(m_pFrame,this,
                                                         m_ucScanStart,m_ucScanStop,
                                                         m_ucLowBit + m_ucHiddenBits,
                                                         m_ucHighBit + m_ucHiddenBits);
    } else { 
      m_pParser = new(m_pEnviron) class ACRefinementScan(m_pFrame,this,
                                                         m_ucScanStart,m_ucScanStop,
                                                         m_ucLowBit + m_ucHiddenBits,
                                                         m_ucHighBit + m_ucHiddenBits);
    }
    break;
  case ACDifferentialProgressive: 
    if (m_ucHighBit == 0) { // The first scan is parsed off by the regular parser.
      m_pParser = new(m_pEnviron) class ACSequentialScan(m_pFrame,this,
                                                         m_ucScanStart,m_ucScanStop,
                                                         m_ucLowBit + m_ucHiddenBits,
                                                         m_ucHighBit + m_ucHiddenBits,
                                                         true);
    } else { 
      m_pParser = new(m_pEnviron) class ACRefinementScan(m_pFrame,this,
                                                         m_ucScanStart,m_ucScanStop,
                                                         m_ucLowBit + m_ucHiddenBits,
                                                         m_ucHighBit + m_ucHiddenBits,
                                                         true);
    }
    break;
  case ACResidualProgressive:  
    if (m_ucHighBit == 0) { // The first scan is parsed off by the regular parser.
      m_pParser = new(m_pEnviron) class ACSequentialScan(m_pFrame,this,
                                                         m_ucScanStart,m_ucScanStop,
                                                         m_ucLowBit + m_ucHiddenBits,
                                                         m_ucHighBit + m_ucHiddenBits,
                                                         false,true);
    } else { 
      m_pParser = new(m_pEnviron) class ACRefinementScan(m_pFrame,this,
                                                         m_ucScanStart,m_ucScanStop,
                                                         m_ucLowBit + m_ucHiddenBits,
                                                         m_ucHighBit + m_ucHiddenBits,
                                                         false,true);
    }
    break;
  case Residual:
    m_pParser = new(m_pEnviron) SequentialScan(m_pFrame,this,
                                               m_ucScanStart,m_ucScanStop,
                                               m_ucLowBit + m_ucHiddenBits,
                                               m_ucHighBit + m_ucHiddenBits,
                                               true,true);
    break;
  case ACResidual:
    m_pParser = new(m_pEnviron) ACSequentialScan(m_pFrame,this,
                                                 m_ucScanStart,m_ucScanStop,
                                                 m_ucLowBit + m_ucHiddenBits,
                                                 m_ucHighBit + m_ucHiddenBits,
                                                 true,true);  
    break;
  case ResidualDCT:
    m_pParser = new(m_pEnviron) SequentialScan(m_pFrame,this,
                                               m_ucScanStart,m_ucScanStop,
                                               m_ucLowBit + m_ucHiddenBits,
                                               m_ucHighBit + m_ucHiddenBits,
                                               false,false,true);
    break; 
  case ACResidualDCT:
    m_pParser = new(m_pEnviron) ACSequentialScan(m_pFrame,this,
                                                 m_ucScanStart,m_ucScanStop,
                                                 m_ucLowBit + m_ucHiddenBits,
                                                 m_ucHighBit + m_ucHiddenBits,
                                                 false,false,true);
    break;
  case JPEG_LS:
    // Depends on the interleaving
    switch(m_ucScanStop) {
    case 0:
      if (m_ucCount != 1)
        JPG_THROW(MALFORMED_STREAM,"Scan::CreateParser",
                  "invalid codestream, found a single comonent scan containing more than one component");
      m_pParser = new(m_pEnviron) class SingleComponentLSScan(m_pFrame,this,
                                                              m_ucScanStart, // NEAR
                                                              m_ucMappingTable,
                                                              m_ucLowBit + m_ucHiddenBits); 
      break;
    case 1:
      m_pParser = new(m_pEnviron) class LineInterleavedLSScan(m_pFrame,this,
                                                              m_ucScanStart,
                                                              m_ucMappingTable,
                                                              m_ucLowBit + m_ucHiddenBits);
      break;
    case 2:
      m_pParser = new(m_pEnviron) class SampleInterleavedLSScan(m_pFrame,this,
                                                                m_ucScanStart,
                                                                m_ucMappingTable,
                                                                m_ucLowBit + m_ucHiddenBits);
      break;
    }
    break;
  default:
    JPG_THROW(NOT_IMPLEMENTED,"Scan::CreateParser",
              "sorry, the coding mode in the codestream is currently not supported");
  }
}