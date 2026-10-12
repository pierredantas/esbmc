<?xml version="1.0" encoding="UTF-8"?>
<project xmlns="http://www.plcopen.org/xml/tc6_0201">
  <fileHeader companyName="X" productVersion="1" creationDateTime="2026-10-12T00:00:00"/>
  <contentHeader name="P"/>
  <types>
    <dataTypes/>
    <pous>
      <pou name="P" pouType="program">
        <!-- The contact is wired from T1's ET, which would be read as T1's Q. -->
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
          </inputVars>
          <localVars>
            <variable name="y">
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
            <block localId="3" typeName="TON" instanceName="T1">
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
              <position x="20" y="10"/>
              <connectionPointIn>
                <connection refLocalId="3" formalParameter="ET"/>
              </connectionPointIn>
              <connectionPointOut/>
              <variable>b</variable>
            </contact>
            <coil localId="5" negated="false" storage="none">
              <position x="100" y="10"/>
              <connectionPointIn>
                <connection refLocalId="4"/>
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
