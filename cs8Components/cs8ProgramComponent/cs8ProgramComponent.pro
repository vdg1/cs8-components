TEMPLATE = subdirs
SUBDIRS = lib \
    # plugin \
    #testBin \
    #extractPrototype \
    Val3CompactApp \
    extractDoc \
   # extractIODefinitions \
    cs8DocEditor \
    testCS8App_ReadWrite \
    testCS8Program \
    testVal3Linter \
    val3PreCompiler \
    val3CreateAPI \
    val3CompilerDeployment \
    chmlib \
    importVPlus


    # val3PreCompilerSettings

extractPrototype.depends=lib
extractDoc.depends=lib
cs8DocEditor.depends=lib
val3PreCompiler.depends=lib
val3CreateAPI.depends=lib
val3CompilerDeployment.depends=lib
saveAsS6.depends=lib
