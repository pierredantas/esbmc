<?xml version="1.0" encoding="UTF-8"?>
<project xmlns="http://www.plcopen.org/xml/tc6_0201">
  <fileHeader companyName="SAFE-LD" productVersion="1" creationDateTime="2026-10-12T00:00:00"/>
  <contentHeader name="P"/>
  <types>
    <dataTypes/>
    <pous>
      <pou name="P" pouType="program">
        <!-- A's R is fed through contact c from B's Q, and B's CU through contact d from A's Q: a cycle through two blocks that no coil consumes. -->
        <interface>
          <inputVars>
            <variable name="a">
              <type>
                <BOOL/>
              </type>
            </variable>
            <variable name="b">
              <type>
                <BOOL/>
              </type>
            </variable>
            <variable name="c">
              <type>
                <BOOL/>
              </type>
            </variable>
            <variable name="d">
              <type>
                <BOOL/>
              </type>
            </variable>
          </inputVars>
          <localVars>
            <variable name="y">
              <type>
                <BOOL/>
              </type>
            </variable>
            <variable name="z">
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
            <block localId="3" typeName="CTU" instanceName="A">
              <position x="50" y="40"/>
              <inputVariables>
                <variable formalParameter="CU">
                  <connectionPointIn>
                    <connection refLocalId="2"/>
                  </connectionPointIn>
                </variable>
                <variable formalParameter="R">
                  <connectionPointIn>
                    <connection refLocalId="4"/>
                  </connectionPointIn>
                </variable>
                <variable formalParameter="PV">
                  <connectionPointIn>
                    <connection refLocalId="9"/>
                  </connectionPointIn>
                </variable>
              </inputVariables>
              <outputVariables>
                <variable formalParameter="Q"/>
                <variable formalParameter="CV"/>
              </outputVariables>
            </block>
            <contact localId="4" negated="false" edge="none">
              <position x="20" y="60"/>
              <connectionPointIn>
                <connection refLocalId="5" formalParameter="Q"/>
              </connectionPointIn>
              <connectionPointOut/>
              <variable>c</variable>
            </contact>
            <block localId="5" typeName="CTU" instanceName="B">
              <position x="50" y="80"/>
              <inputVariables>
                <variable formalParameter="CU">
                  <connectionPointIn>
                    <connection refLocalId="6"/>
                  </connectionPointIn>
                </variable>
                <variable formalParameter="PV">
                  <connectionPointIn>
                    <connection refLocalId="9"/>
                  </connectionPointIn>
                </variable>
              </inputVariables>
              <outputVariables>
                <variable formalParameter="Q"/>
                <variable formalParameter="CV"/>
              </outputVariables>
            </block>
            <contact localId="6" negated="false" edge="none">
              <position x="20" y="100"/>
              <connectionPointIn>
                <connection refLocalId="3" formalParameter="Q"/>
              </connectionPointIn>
              <connectionPointOut/>
              <variable>d</variable>
            </contact>
            <inVariable localId="9">
              <position x="30" y="120"/>
              <expression>3</expression>
            </inVariable>
            <coil localId="7" negated="false" storage="none">
              <position x="100" y="10"/>
              <connectionPointIn>
                <connection refLocalId="2"/>
              </connectionPointIn>
              <connectionPointOut/>
              <variable>y</variable>
            </coil>
          </LD>
        </body>
      </pou>
    </pous>
  </types>
  <instances/>
</project>
