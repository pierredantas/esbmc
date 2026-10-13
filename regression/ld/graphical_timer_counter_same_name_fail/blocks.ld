<?xml version="1.0" encoding="UTF-8"?>
<project xmlns="http://www.plcopen.org/xml/tc6_0201">
  <fileHeader companyName="X" productVersion="1" creationDateTime="2026-10-12T00:00:00"/>
  <contentHeader name="P"/>
  <types>
    <dataTypes/>
    <pous>
      <pou name="P" pouType="program">
        <!-- TON X and CTU X would both write the generated pin X__Q. -->
        <interface>
          <inputVars>
            <variable name="a">
              <type>
                <BOOL/>
              </type>
            </variable>
            <variable name="c">
              <type>
                <BOOL/>
              </type>
            </variable>
            <variable name="r">
              <type>
                <BOOL/>
              </type>
            </variable>
          </inputVars>
          <localVars>
            <variable name="tq">
              <type>
                <BOOL/>
              </type>
            </variable>
            <variable name="cq">
              <type>
                <BOOL/>
              </type>
            </variable>
          </localVars>
        </interface>
        <body>
          <LD>
            <leftPowerRail localId="1">
              <position x="0" y="0"/>
              <connectionPointOut/>
            </leftPowerRail>
            <contact localId="2" negated="false" edge="none">
              <position x="20" y="10"/>
              <connectionPointIn>
                <connection refLocalId="1"/>
              </connectionPointIn>
              <connectionPointOut/>
              <variable>a</variable>
            </contact>
            <block localId="3" typeName="TON" instanceName="X">
              <position x="50" y="10"/>
              <inputVariables>
                <variable formalParameter="IN">
                  <connectionPointIn>
                    <connection refLocalId="2"/>
                  </connectionPointIn>
                </variable>
                <variable formalParameter="PT">
                  <connectionPointIn>
                    <connection refLocalId="9"/>
                  </connectionPointIn>
                </variable>
              </inputVariables>
              <outputVariables>
                <variable formalParameter="Q"/>
                <variable formalParameter="ET"/>
              </outputVariables>
            </block>
            <inVariable localId="9">
              <position x="30" y="30"/>
              <expression>T#100ms</expression>
            </inVariable>
            <contact localId="4" negated="false" edge="none">
              <position x="20" y="60"/>
              <connectionPointIn>
                <connection refLocalId="1"/>
              </connectionPointIn>
              <connectionPointOut/>
              <variable>c</variable>
            </contact>
            <contact localId="6" negated="false" edge="none">
              <position x="20" y="70"/>
              <connectionPointIn>
                <connection refLocalId="1"/>
              </connectionPointIn>
              <connectionPointOut/>
              <variable>r</variable>
            </contact>
            <block localId="5" typeName="CTU" instanceName="X">
              <position x="50" y="60"/>
              <inputVariables>
                <variable formalParameter="CU">
                  <connectionPointIn>
                    <connection refLocalId="4"/>
                  </connectionPointIn>
                </variable>
                <variable formalParameter="R">
                  <connectionPointIn>
                    <connection refLocalId="6"/>
                  </connectionPointIn>
                </variable>
                <variable formalParameter="PV">
                  <connectionPointIn>
                    <connection refLocalId="10"/>
                  </connectionPointIn>
                </variable>
              </inputVariables>
              <outputVariables>
                <variable formalParameter="Q"/>
                <variable formalParameter="CV"/>
              </outputVariables>
            </block>
            <inVariable localId="10">
              <position x="30" y="80"/>
              <expression>1</expression>
            </inVariable>
            <coil localId="7" negated="false" storage="none">
              <position x="100" y="10"/>
              <connectionPointIn>
                <connection refLocalId="3" formalParameter="Q"/>
              </connectionPointIn>
              <connectionPointOut/>
              <variable>tq</variable>
            </coil>
            <coil localId="8" negated="false" storage="none">
              <position x="100" y="60"/>
              <connectionPointIn>
                <connection refLocalId="5" formalParameter="Q"/>
              </connectionPointIn>
              <connectionPointOut/>
              <variable>cq</variable>
            </coil>
          </LD>
        </body>
      </pou>
    </pous>
  </types>
  <instances/>
</project>
