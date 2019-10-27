//
//  ShaderManager.cpp
//
//  Created by Cam Stocker on 4/23/19.
//

/***********************
 HOW TO ADD A SHADER:
 1) Create a #define in ShaderManager.h that will be used to identify your shader
 2) Add an init function in ShaderManager.cpp and put your initialization code there
    - be sure to add a prototype of this function in ShaderManager.h
 3) Call your init function from initShaders in ShaderManager.cpp and save it to the
    respective location in shaderMap. See example
 
 HOW TO USE A SHADER IN THE RENDER LOOP
 1) first, call shaderManager.setCurrentShader(int name) to set the current shader
 2) To retrieve the current shader, call shaderManager.getCurrentShader()
 3) Use the return value of getCurrentShader() to render
 ***********************/

#include <stdio.h>
#include <iostream>

#include "ShaderManager.h"

void ShaderManager::initShaders() {
    shaderMap[VPLPROG] = initVPLProgShader();
    shaderMap[RENDERPROG] = initRenderShader();
}

shared_ptr<Program> ShaderManager::initVPLProgShader() {
//    // Initialize the GLSL program.
    std::shared_ptr<Program> prog = make_shared<Program>();
    
    prog->setVerbose(true);
    prog->setShaderNames(resourceDirectory + "/shaders/vpl_vert.glsl", resourceDirectory + "/shaders/vpl_frag.glsl");
    
    if (!prog->init())
    {
        cerr << "One or more shaders failed to compile... exiting!" << endl;
        exit(1);
    }
    
    prog->addUniform("P");
    prog->addUniform("V");
    prog->addUniform("M");
    prog->addUniform("baseColor");
    prog->addUniform("lightPos");
    prog->addAttribute("vertPos");
    prog->addAttribute("vertNor");
    
    return prog;
}

shared_ptr<Program> ShaderManager::initRenderShader() {
//    // Initialize the GLSL program.
    std::shared_ptr<Program> prog = make_shared<Program>();
    
    prog->setVerbose(true);
    prog->setShaderNames(resourceDirectory + "/shaders/render_vert.glsl", resourceDirectory + "/shaders/render_frag.glsl");
    
    if (!prog->init())
    {
        cerr << "One or more shaders failed to compile... exiting!" << endl;
        exit(1);
    }
    
    prog->addUniform("P");
    prog->addUniform("V");
    prog->addUniform("M");
    prog->addUniform("VPLpositions");
    prog->addUniform("VPLcolors");
    prog->addUniform("VPLresolution");
    prog->addUniform("baseColor");
    prog->addAttribute("vertPos");
    prog->addAttribute("vertNor");
    
    return prog;
}
