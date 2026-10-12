<?xml version="1.0" encoding="utf-8"?>
<project xmlns="http://www.plcopen.org/xml/tc6_0200" xmlns:xhtml="http://www.w3.org/1999/xhtml">
  <fileHeader companyName="ESBMC" productName="TV REAL fixture" productVersion="1.0" creationDateTime="2026-10-07T00:00:00"/>
  <contentHeader name="tv_real"><coordinateInfo><fbd><scaling x="1" y="1"/></fbd><ld><scaling x="1" y="1"/></ld><sfc><scaling x="1" y="1"/></sfc></coordinateInfo></contentHeader>
  <types>
    <dataTypes/>
    <pous>
      <pou name="prog" pouType="program">
        <interface>
          <inputVars>
            <variable name="a"><type><REAL/></type></variable>
            <variable name="b"><type><REAL/></type></variable>
            <variable name="c"><type><REAL/></type></variable>
          </inputVars>
          <outputVars>
            <variable name="q"><type><BOOL/></type></variable>
          </outputVars>
          <localVars><variable name="sum0"><type><derived name="sum"/></type></variable></localVars>
        </interface>
        <body>
          <LD>
            <inVariable localId="1" height="30" width="50" negated="false">
              <position x="0" y="0"/>
              <connectionPointOut><relPosition x="50" y="15"/></connectionPointOut>
              <expression>a</expression>
            </inVariable>
            <inVariable localId="2" height="30" width="50" negated="false">
              <position x="0" y="40"/>
              <connectionPointOut><relPosition x="50" y="15"/></connectionPointOut>
              <expression>b</expression>
            </inVariable>
            <inVariable localId="3" height="30" width="50" negated="false">
              <position x="0" y="80"/>
              <connectionPointOut><relPosition x="50" y="15"/></connectionPointOut>
              <expression>c</expression>
            </inVariable>
            <block localId="4" typeName="sum" instanceName="sum0" height="80" width="100">
              <position x="100" y="0"/>
              <inputVariables>
                <variable formalParameter="IN1"><connectionPointIn><relPosition x="0" y="20"/><connection refLocalId="1"><position x="100" y="20"/><position x="50" y="15"/></connection></connectionPointIn></variable>
                <variable formalParameter="IN2"><connectionPointIn><relPosition x="0" y="40"/><connection refLocalId="2"><position x="100" y="40"/><position x="50" y="15"/></connection></connectionPointIn></variable>
                <variable formalParameter="IN3"><connectionPointIn><relPosition x="0" y="60"/><connection refLocalId="3"><position x="100" y="60"/><position x="50" y="15"/></connection></connectionPointIn></variable>
              </inputVariables>
              <inOutVariables/>
              <outputVariables>
                <variable formalParameter="OUT"><connectionPointOut><relPosition x="100" y="40"/></connectionPointOut></variable>
              </outputVariables>
            </block>
            <outVariable localId="5" height="30" width="50" negated="false">
              <position x="250" y="30"/>
              <connectionPointIn><relPosition x="0" y="15"/><connection refLocalId="4" formalParameter="OUT"><position x="100" y="20"/><position x="50" y="15"/></connection></connectionPointIn>
              <expression>q</expression>
            </outVariable>
          </LD>
        </body>
      </pou>
      <pou name="sum" pouType="functionBlock">
        <interface>
          <inputVars>
            <variable name="IN1"><type><REAL/></type></variable>
            <variable name="IN2"><type><REAL/></type></variable>
            <variable name="IN3"><type><REAL/></type></variable>
          </inputVars>
          <outputVars><variable name="OUT"><type><BOOL/></type></variable></outputVars>
        </interface>
        <body><ST><xhtml:p><![CDATA[OUT := (IN1 + IN2) = IN3;
]]></xhtml:p></ST></body>
      </pou>
    </pous>
  </types>
  <instances>
    <configurations>
      <configuration name="Config0">
        <resource name="Res0">
          <task name="task0" priority="0" interval="T#20ms"><pouInstance name="instance0" typeName="prog"/></task>
        </resource>
      </configuration>
    </configurations>
  </instances>
</project>
