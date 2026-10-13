<?xml version="1.0" encoding="UTF-8"?>
<project xmlns="http://www.plcopen.org/xml/tc6_0201">
  <fileHeader companyName="SAFE-LD" productVersion="1.0" creationDateTime="2026-10-12"/>
  <contentHeader name="P"/>
  <types><dataTypes/><pous>
    <pou name="BIG" pouType="functionBlock">
      <interface>
        <inputVars><variable name="IN"><type><INT/></type></variable></inputVars>
        <outputVars><variable name="OUT"><type><BOOL/></type></variable></outputVars>
      </interface>
      <body><ST><![CDATA[
OUT := IN > 2;
]]></ST></body>
    </pou>
    <pou name="P" pouType="program">
      <!-- A user FB's IN is a data input, so wiring T1's ET into it is accepted; ET reaches 5 > 2. -->
      <interface>
        <inputVars><variable name="a"><type><BOOL/></type></variable></inputVars>
        <localVars><variable name="y"><type><BOOL/></type></variable><variable name="big"><type><BOOL/></type></variable></localVars>
      </interface>
      <body>
        <LD>
          <leftPowerRail localId="1"><position x="0" y="0"/><connectionPointOut/></leftPowerRail>
          <contact localId="2" negated="false" edge="none"><position x="20" y="10"/>
            <connectionPointIn><connection refLocalId="1"/></connectionPointIn><connectionPointOut/><variable>a</variable></contact>
          <block localId="3" typeName="TON" instanceName="T1"><position x="50" y="10"/>
            <inputVariables>
              <variable formalParameter="IN"><connectionPointIn><connection refLocalId="2"/></connectionPointIn></variable>
              <variable formalParameter="PT"><connectionPointIn><connection refLocalId="9"/></connectionPointIn></variable>
            </inputVariables>
            <outputVariables><variable formalParameter="Q"/><variable formalParameter="ET"/></outputVariables>
          </block>
          <inVariable localId="9"><position x="30" y="30"/><expression>5</expression></inVariable>
          <coil localId="5" negated="false" storage="none"><position x="150" y="10"/>
            <connectionPointIn><connection refLocalId="3" formalParameter="Q"/></connectionPointIn><connectionPointOut/><variable>y</variable></coil>
          <block localId="6" typeName="BIG" instanceName="B1"><position x="50" y="60"/>
            <inputVariables>
              <variable formalParameter="IN"><connectionPointIn><connection refLocalId="3" formalParameter="ET"/></connectionPointIn></variable>
            </inputVariables>
            <outputVariables><variable formalParameter="OUT"/></outputVariables>
          </block>
          <outVariable localId="7"><position x="150" y="60"/><expression>big</expression>
            <connectionPointIn><connection refLocalId="6" formalParameter="OUT"/></connectionPointIn></outVariable>
        </LD>
      </body>
    </pou>
  </pous></types>
  <instances/>
</project>
