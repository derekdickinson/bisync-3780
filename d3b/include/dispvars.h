char *mydat;

DISPVAR dv[]=
{
  {
    SMODE,
    NULL, /* dispforms[BUFDV] */
    0, /* Maxlen is a don't care here. */
    1, /* statmask */
    sndmodename,
    NULL,
    0xff
  },
  {
    STRV,
    NULL, /* dispforms[DIRDV] */
    27,
    1,
    NULL,
    (VALTYP *) curdir,
    0xff
  },
  {
    LONGV,
    NULL, /* dispforms[INDV] */
    0, /* Maxlen is a don't care here. */
    4, /* statmask */
    NULL,
    (VALTYP *) &datain, /* Address */
    0xffffffffL
  },
  {
    LONGV,
    NULL, /* dispforms[OUTDV] */
    0, /* Maxlen is a don't care here. */
    2, /* statmask */
    NULL,
    (VALTYP *) &dataout, /* Address */
    0xffffffffL
  },
  {
    BINDV,
    NULL, /* dispforms[PTRAUTODV] */
    0, /* Maxlen is a don't care here. */
    1, /* statmask */
    noyes,
    (VALTYP *) &ptr.autoname,
    0xff
  },
  {
    STRV,
    NULL, /* dispforms[PTRNAMEDV] */
    25, /* Maxlen is a don't care here. */
    1, /* statmask */
    NULL,
    (VALTYP *) &ptr.filename,
    0xff
  },
  {
    BINDV,
    NULL, /* dispforms[PTRSTDDV] */
    0, /* Maxlen is a don't care here. */
    1, /* statmask */
    rcvcnv,
    (VALTYP *) &ptr.stdcnv,
    0xff
  },
  {
    BINDV,
    NULL, /* dispforms[PTRTRANDV] */
    0, /* Maxlen is a don't care here. */
    1, /* statmask */
    rcvcnv,
    (VALTYP *) &ptr.trancnv,
    0xff
  },
  {
    BINDV,
    NULL, /* dispforms[PUNAUTODV] */
    0, /* Maxlen is a don't care here. */
    1, /* statmask */
    noyes,
    (VALTYP *) &pun.autoname,
    0xff
  },
  {
    STRV,
    NULL, /* dispforms[PUNNAMEDV] */
    25, /* Maxlen is a don't care here. */
    1, /* statmask */
    NULL,
    (VALTYP *) &pun.filename,
    0xff
  },
  {
    BINDV,
    NULL, /* dispforms[PUNSTDDV] */
    0, /* Maxlen is a don't care here. */
    1, /* statmask */
    rcvcnv,
    (VALTYP *) &pun.stdcnv,
    0xff
  },
  {
    BINDV,
    NULL, /* dispforms[PUNTRANDV] */
    0, /* Maxlen is a don't care here. */
    1, /* statmask */
    rcvcnv,
    (VALTYP *) &pun.trancnv,
    0xff
  },
  {
    INTV,
    NULL, /* dispforms[RBLKDV] */
    0, /* Maxlen is a don't care here. */
    4, /* statmask */
    NULL,
    (VALTYP *) &rblkcnt, /* Address */
    0xffffL
  },
  {
    INTV,
    NULL, /* dispforms[RENQDV] */
    0, /* Maxlen is a don't care here. */
    4, /* statmask */
    NULL,
    (VALTYP *) &renqcnt, /* Address */
    0xffffL
  },
  {
    INTV,
    NULL, /* dispforms[RNAKDV] */
    0, /* Maxlen is a don't care here. */
    4, /* statmask */
    NULL,
    (VALTYP *) &rnakcnt, /* Address */
    0xffffL
  },
  {
    INTV,
    NULL, /* dispforms[SBLKDV] */
    0, /* Maxlen is a don't care here. */
    2, /* statmask */
    NULL,
    (VALTYP *) &sblkcnt, /* Address */
    0xffffL
  },
  {
    SDEST,
    NULL, /* dispforms[ACTPRINDV] */
    0, /* Maxlen is a don't care here. */
    1, /* statmask */
    prinpunch,
    NULL, /* Address, sendlist->prindest  */
    0xff
  },
  {
    STRV,
    NULL, /* dispforms[SNDNAMEDV] */
    27, /* Maxlen is a don't care here. */
    1, /* statmask */
    NULL,
    (VALTYP *) &senfilname,
    0xff
  },
  {
    INTV,
    NULL, /* dispforms[SENQDV] */
    0, /* Maxlen is a don't care here. */
    2, /* statmask */
    NULL,
    (VALTYP *) &senqcnt, /* Address */
    0xffffL
  },
  {
    INTV,
    NULL, /* dispforms[SNAKDV] */
    0, /* Maxlen is a don't care here. */
    2, /* statmask */
    NULL,
    (VALTYP *) &snakcnt, /* Address */
    0xffff
  },
  {
    BINDV,
    NULL, /* dispforms[SRVARDV] */
    0, /* Maxlen is a don't care here. */
    1, /* statmask */
    srs,
    (VALTYP *) &srd,
    0xff
  },
  {
    BINDV,
    NULL, /* dispforms[STATMDV] */
    0, /* Maxlen doesn't matter here. */
    1, /* statmask */
    statmess,
    (VALTYP *) &statval,
    0xff
  },
  {
    BINDV,
    NULL, /* dispforms[STDV] */
    0, /* Maxlen is a don't care here. */
    1, /* statmask */
    sts,
    (VALTYP *) &std,
    0xff
  },
  {
    DATEV,
    NULL, /* dispforms[DATEDV] */
    0,
    1,
    NULL,
    NULL,
    0
  },
  {
    TIMEV,
    NULL, /* dispforms[TIMEDV] */
    0,
    1,
    NULL,
    NULL,
    0
  }
};

char *dispforms[sizeof(dv)/sizeof(DISPVAR)];
