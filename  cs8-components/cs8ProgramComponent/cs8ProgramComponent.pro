TEMPLATE = subdirs
SUBDIRS = lib \
    # plugin \
    #testBin \
    extractPrototype \
    extractDoc \
    cs8DocEditor \
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
