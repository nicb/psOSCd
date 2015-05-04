<!doctype refentry PUBLIC "-//OASIS//DTD DocBook V4.1//EN" [

<!-- Process this file with docbook-to-man to generate an nroff manual
     page: `docbook-to-man manpage.sgml > manpage.1'.  You may view
     the manual page with: `docbook-to-man manpage.sgml | nroff -man |
     less'.  A typical entry in a Makefile or Makefile.am is:

manpage.1: manpage.sgml
	docbook-to-man $< > $@


	The docbook-to-man binary is found in the docbook-to-man package.
	Please remember that if you create the nroff version in one of the
	debian/rules file targets (such as build), you will need to include
	docbook-to-man in your Build-Depends control field.

  -->

  <!-- Fill in your name for FIRSTNAME and SURNAME. -->
  <!ENTITY dhfirstname "<firstname>Nicola</firstname>">
  <!ENTITY dhsurname   "<surname>Bernardini</surname>">
  <!-- Please adjust the date whenever revising the manpage. -->
  <!ENTITY dhdate      "<date>April 23, 2010</date>">
  <!-- SECTION should be 1-8, maybe w/ subsection other parameters are
       allowed: see man(7), man(1). -->
  <!ENTITY dhsection   "<manvolnum>8</manvolnum>">
  <!ENTITY dhemail     "<email>nicb@sme-ccppd.org</email>">
  <!ENTITY dhusername  "Nicola Bernardini">
  <!ENTITY dhucpackage "<refentrytitle>PSOSCD</refentrytitle>">
  <!ENTITY dhpackage   "psOSCd">

  <!ENTITY debian      "<productname>Debian</productname>">
  <!ENTITY gnu         "<acronym>GNU</acronym>">
  <!ENTITY gpl         "&gnu; <acronym>GPL</acronym>">
]>

<refentry>
  <refentryinfo>
    <address>
      &dhemail;
    </address>
    <author>
      &dhfirstname;
      &dhsurname;
    </author>
    <copyright>
      <year>2003</year>
      <holder>&dhusername;</holder>
    </copyright>
    &dhdate;
  </refentryinfo>
  <refmeta>
    &dhucpackage;

    &dhsection;
  </refmeta>
  <refnamediv>
    <refname>&dhpackage;</refname>

    <refpurpose>a PhaseSpace Impulse to OSC protocol middleware</refpurpose>
  </refnamediv>
  <refsynopsisdiv>
    <cmdsynopsis>
      <command>&dhpackage;</command>

      <arg><option>-C, --osc_control_port <replaceable>OSC control port number</replaceable></option></arg>
      <arg><option>-p, --phasespace_server <replaceable>phasespace host to connect to</replaceable></option></arg>
      <arg><option>-o, --osc_server <replaceable>OSC server to connect to</replaceable></option></arg>
      <arg><option>-m, --num_markers <replaceable>number of markers to watch</replaceable></option></arg>
      <arg><option>-f, --frequency <replaceable>streaming frequency</replaceable></option></arg>
      <arg><option>-s, --slave</option></arg>
      <arg><option>-c, --config <replaceable>path to configuration file</replaceable></option></arg>
      <arg><option>-v, --verbose</option></arg>
    </cmdsynopsis>
  </refsynopsisdiv>
  <refsect1>
    <title>DESCRIPTION</title>

    <para>This manual page documents briefly the
      <command>&dhpackage;</command> command.</para>

    <para>This manual page was written for the &debian; distribution
      because the original program does not have a manual page.
      Instead, it has documentation in the &gnu;
      <application>Info</application> format; see below.</para>

    <para><command>&dhpackage;</command>
    is a daemon that that will hook up to a PhaseSpace Impulse Motion Capture
    server and transform its input into OSC (http://opensoundcontrol.org) data triplets
    to be used by any OSC-capable real-time software.</para>

  </refsect1>
  <refsect1>
    <title>OPTIONS</title>

    <para>These programs follow the usual &gnu; command line syntax,
      with long options starting with two dashes (`-').  A summary of
      options is included below.  For a complete description, see the
      <application>Info</application> files.</para>

    <variablelist>
      <varlistentry>
        <term><option>-C</option>
          <option>--osc_control_port</option>
        </term>
        <listitem>
          <para>port that &dhpackage is listening to to receive OSC commands
          (for configuration and operation).</para>
        </listitem>
      </varlistentry>
      <varlistentry>
        <term><option>-p</option>
          <option>--phasespace_server</option>
        </term>
        <listitem>
          <para>phasespace host to connect to (generally in <b>host</b> notation).</para>
        </listitem>
      </varlistentry>
      <varlistentry>
        <term><option>-o</option>
          <option>--osc_server</option>
        </term>
        <listitem>
          <para>OSC server to connect to (generally in <b>host:port</b> notation).</para>
        </listitem>
      </varlistentry>
      <varlistentry>
        <term><option>-m</option>
          <option>--num_markers</option>
        </term>
        <listitem>
          <para>number of markers to watch</para>
        </listitem>
      </varlistentry>
      <varlistentry>
        <term><option>-f</option>
          <option>--frequency</option>
        </term>
        <listitem>
          <para>streaming system frequency (max: 480.0/sec)</para>
        </listitem>
      </varlistentry>
      <varlistentry>
        <term><option>-s</option>
          <option>--slave</option>
        </term>
        <listitem>
          <para>run as slave</para>
        </listitem>
      </varlistentry>
      <varlistentry>
        <term><option>-c</option>
          <option>--config</option>
        </term>
        <listitem>
          <para>path to configuration file (default: /etc/&dhpackage/&dhpackage.conf)</para>
        </listitem>
      </varlistentry>
      <varlistentry>
        <term><option>-v</option>
          <option>--verbose</option>
        </term>
        <listitem>
          <para>run with verbose comments (in /var/log/syslog)</para>
        </listitem>
      </varlistentry>
    </variablelist>
  </refsect1>
  <refsect1>
    <title>EXAMPLE</title>
				<para>A typical usage is:</para>
				<para>
				psOSCd -p localhost -o 192.168.1.21:4600 -n 6 -v
				</para>
				<para>which means: connect to the phasespace server present on the same machine,
				watch 6 markers, send them over to the OSC server on the 192.168.1.21 machine
				on port 4600 (and run verbose).</para>
  </refsect1>
  <refsect1>
    <title>AUTHOR</title>
    <para>This manual page was written by &dhusername; &dhemail; for
      the &debian; system (and may be used by others).  Permission is
      granted to copy, distribute and/or modify this document under
      the terms of the &gnu; General Public License, Version 2 any
      later version published by the Free Software Foundation.
    </para>
    <para>
      On Debian systems, the complete text of the GNU General Public
      License can be found in /usr/share/common-licenses/GPL.
    </para>

  </refsect1>
</refentry>

<!-- Keep this comment at the end of the file
Local variables:
mode: sgml
sgml-omittag:t
sgml-shorttag:t
sgml-minimize-attributes:nil
sgml-always-quote-attributes:t
sgml-indent-step:2
sgml-indent-data:t
sgml-parent-document:nil
sgml-default-dtd-file:nil
sgml-exposed-tags:nil
sgml-local-catalogs:nil
sgml-local-ecat-files:nil
End:
-->
