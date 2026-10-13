<?xml version="1.0" encoding="UTF-8"?>
<project xmlns="http://www.plcopen.org/xml/tc6_0201">
  <fileHeader companyName="SAFE-LD" productVersion="1.0" creationDateTime="2026-10-12"/>
  <contentHeader name="P"/>
  <types><dataTypes/><pous>
    <pou name="PASS" pouType="functionBlock">
      <interface>
        <inputVars><variable name="A"><type><BOOL/></type></variable></inputVars>
        <outputVars><variable name="Q"><type><BOOL/></type></variable></outputVars>
      </interface>
      <body><ST><![CDATA[
Q := A;
]]></ST></body>
    </pou>
    <pou name="P" pouType="program">
      <!-- The expression reads the generated name pf2 once its spaces are trimmed. -->
      <interface>
        <inputVars><variable name="a"><type><BOOL/></type></variable></inputVars>
        <localVars><variable name="y"><type><BOOL/></type></variable><variable name="z"><type><BOOL/></type></variable></localVars>
      </interface>
      <body>
        <LD>
          <leftPowerRail localId="1"><position x="0" y="0"/><connectionPointOut/></leftPowerRail>
          <contact localId="2" negated="false" edge="none"><position x="20" y="10"/>
            <connectionPointIn><connection refLocalId="1"/></connectionPointIn><connectionPointOut/><variable>a</variable></contact>
          <coil localId="3" negated="false" storage="none"><position x="100" y="10"/>
            <connectionPointIn><connection refLocalId="2"/></connectionPointIn><connectionPointOut/><variable>y</variable></coil>
          <inVariable localId="8"><position x="0" y="50"/><expression> pf2 </expression></inVariable>
          <block localId="6" typeName="PASS" instanceName="B1"><position x="50" y="60"/>
            <inputVariables>
              <variable formalParameter="A"><connectionPointIn><connection refLocalId="8"/></connectionPointIn></variable>
            </inputVariables>
            <outputVariables><variable formalParameter="Q"/></outputVariables>
          </block>
          <outVariable localId="7"><position x="150" y="60"/><expression>z</expression>
            <connectionPointIn><connection refLocalId="6" formalParameter="Q"/></connectionPointIn></outVariable>
        </LD>
      </body>
    </pou>
  </pous></types>
  <instances/>
</project>
