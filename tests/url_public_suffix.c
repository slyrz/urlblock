#define _GNU_SOURCE
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "urlblock.h"

struct test_case {
  const char *inp;
  const char *exp;
};

static struct test_case cases[] = {
  {"COM", NULL},
  {"example.COM", "example.com"},
  {"WwW.example.COM", "example.com"},
  // Leading dot.
  {".com", NULL},
  {".example", NULL},
  {".example.com", NULL},
  {".example.example", NULL},
  // Unlisted TLD.
  {"example", NULL},
  {"example.example", "example.example"},
  {"b.example.example", "example.example"},
  {"a.b.example.example", "example.example"},
  // TLD with only 1 rule.
  {"biz", NULL},
  {"domain.biz", "domain.biz"},
  {"b.domain.biz", "domain.biz"},
  {"a.b.domain.biz", "domain.biz"},
  // TLD with some 2-level rules.
  {"com", NULL},
  {"example.com", "example.com"},
  {"b.example.com", "example.com"},
  {"a.b.example.com", "example.com"},
  {"uk.com", NULL},
  {"example.uk.com", "example.uk.com"},
  {"b.example.uk.com", "example.uk.com"},
  {"a.b.example.uk.com", "example.uk.com"},
  {"test.ac", "test.ac"},
  // TLD with only 1 (wildcard) rule.
  {"cy", NULL},
  {"c.cy", NULL},
  {"b.c.cy", "b.c.cy"},
  {"a.b.c.cy", "b.c.cy"},
  // More complex TLD.
  {"jp", NULL},
  {"test.jp", "test.jp"},
  {"www.test.jp", "test.jp"},
  {"ac.jp", NULL},
  {"test.ac.jp", "test.ac.jp"},
  {"www.test.ac.jp", "test.ac.jp"},
  {"kyoto.jp", NULL},
  {"test.kyoto.jp", "test.kyoto.jp"},
  {"ide.kyoto.jp", NULL},
  {"b.ide.kyoto.jp", "b.ide.kyoto.jp"},
  {"a.b.ide.kyoto.jp", "b.ide.kyoto.jp"},
  {"c.kobe.jp", NULL},
  {"b.c.kobe.jp", "b.c.kobe.jp"},
  {"a.b.c.kobe.jp", "b.c.kobe.jp"},
  {"city.kobe.jp", "city.kobe.jp"},
  {"www.city.kobe.jp", "city.kobe.jp"},
  // TLD with a wildcard rule and exceptions.
  {"ck", NULL},
  {"test.ck", NULL},
  {"b.test.ck", "b.test.ck"},
  {"a.b.test.ck", "b.test.ck"},
  {"www.ck", "www.ck"},
  {"www.www.ck", "www.ck"},
  // US K12.
  {"us", NULL},
  {"test.us", "test.us"},
  {"www.test.us", "test.us"},
  {"ak.us", NULL},
  {"test.ak.us", "test.ak.us"},
  {"www.test.ak.us", "test.ak.us"},
  {"k12.ak.us", NULL},
  {"test.k12.ak.us", "test.k12.ak.us"},
  {"www.test.k12.ak.us", "test.k12.ak.us"},
  // IDN labels.
  {"食狮.com.cn", "食狮.com.cn"},
  {"食狮.公司.cn", "食狮.公司.cn"},
  {"www.食狮.公司.cn", "食狮.公司.cn"},
  {"shishi.公司.cn", "shishi.公司.cn"},
  {"公司.cn", NULL},
  {"食狮.中国", "食狮.中国"},
  {"www.食狮.中国", "食狮.中国"},
  {"shishi.中国", "shishi.中国"},
  {"中国", NULL},
  // Autogenerated test cases.
  {"qgrbr.accident-prevention.aero", "qgrbr.accident-prevention.aero"},
  {"ae.pmezhukkz.yoshida.saitama.jp", "pmezhukkz.yoshida.saitama.jp"},
  {"oirase.aomori.jp", NULL},
  {"kkeifzfxw.oyuzsic.serrdw.kurashiki.okayama.jp", "serrdw.kurashiki.okayama.jp"},
  {"vqdubua.web.do", "vqdubua.web.do"},
  {"pmxlpvoast.qyxlsr.kitakata.fukushima.jp", "qyxlsr.kitakata.fukushima.jp"},
  {"vwfdg..og.it", NULL},
  {"rllznrr.q.kkj.nic.in", "kkj.nic.in"},
  {"benib.bbvujfnmr.od.商城", "od.商城"},
  {"taiwrudgu.gliding.aero", "taiwrudgu.gliding.aero"},
  {"toho.fukuoka.jp", NULL},
  {"wrkbkv.ppd.ora.gunma.jp", "ppd.ora.gunma.jp"},
  {"hjartdal.no", NULL},
  {"hpzi.a..langevåg.no", NULL},
  {"eowoqpdu.skjåk.no", "eowoqpdu.skjåk.no"},
  {"kongsvinger.no", NULL},
  {"nagatoro.saitama.jp", NULL},
  {"tm.km", NULL},
  {"okuizumo.shimane.jp", NULL},
  {"ao.nwgx.ind.gt", "nwgx.ind.gt"},
  {"mjefavu.uqlcpmirn.zohdlcft.ks.us", "zohdlcft.ks.us"},
  {"uyuhklb.ntt", "uyuhklb.ntt"},
  {"mwp.j.rfndejih.edu.sv", "rfndejih.edu.sv"},
  {"alabama.museum", NULL},
  {"gold", NULL},
  {".miyakonojo.miyazaki.jp", NULL},
  {"fwbulw.phpdjcxez.is-an-entertainer.com", "phpdjcxez.is-an-entertainer.com"},
  {"dvldcxssus..servebbs.net", NULL},
  {"plo.ps", NULL},
  {"org.uk", NULL},
  {"hdstbkumt.k12.co.us", "hdstbkumt.k12.co.us"},
  {"oxktnnj.jqtvqlf.pv.it", "jqtvqlf.pv.it"},
  {"oekza.mlvaeo.zkz.pomorze.pl", "zkz.pomorze.pl"},
  {"qlmkpt.aioi.hyogo.jp", "qlmkpt.aioi.hyogo.jp"},
  {"matsukawa.nagano.jp", NULL},
  {"ishikawa.fukushima.jp", NULL},
  {"zcztriear.peexnizjlx.hdfcbank", "peexnizjlx.hdfcbank"},
  {"rptylznkhj.eccpoof.zn.co.ve", "zn.co.ve"},
  {"mziapvd.net.mt", "mziapvd.net.mt"},
  {"jeonnam.kr", NULL},
  {"gqabcm.si.it", "gqabcm.si.it"},
  {"xujvek.k12.me.us", "xujvek.k12.me.us"},
  {"em..desbtdabh.mytis.ru", NULL},
  {"xhkt.ivakwyq.kll.alaheadju.no", "kll.alaheadju.no"},
  {"nodoamoj.gxkrvn.c.host", "c.host"},
  {"ko.gen.nz", "ko.gen.nz"},
  {"ifsqhezheu.kj.fans", "kj.fans"},
  {".zcnlyvrqnc.ky", NULL},
  {"ybfgdq.shiojiri.nagano.jp", "ybfgdq.shiojiri.nagano.jp"},
  {".co.nz", NULL},
  {"avnh.zwbie.brmfygq.siellak.no", "brmfygq.siellak.no"},
  {"kyqvz.ichikawamisato.yamanashi.jp", "kyqvz.ichikawamisato.yamanashi.jp"},
  {"g.qqprf.cc.ct.us", "qqprf.cc.ct.us"},
  {"nsawzogry.hnpoo.yn.ogasawara.tokyo.jp", "yn.ogasawara.tokyo.jp"},
  {"gliwice.pl", NULL},
  {"pv.forli-cesena.it", "pv.forli-cesena.it"},
  {"it.monzaedellabrianza.it", "it.monzaedellabrianza.it"},
  {"mjjwifiux.chino.nagano.jp", "mjjwifiux.chino.nagano.jp"},
  {"yftbtxb.washingtondc.museum", "yftbtxb.washingtondc.museum"},
  {"nczduozxi.zn.xfzph.shimotsuma.ibaraki.jp", "xfzph.shimotsuma.ibaraki.jp"},
  {"omdtbopyl.gublgrz.f.dontexist.com", "f.dontexist.com"},
  {"karelia.su", NULL},
  {"fu.mizuho.tokyo.jp", "fu.mizuho.tokyo.jp"},
  {"zbbdetxqxx.admvlmyj.oeoedx.adult", "oeoedx.adult"},
  {"v.org.sn", "v.org.sn"},
  {"royqhph.stwzknq.til.net.nf", "til.net.nf"},
  {"yoga", NULL},
  {"e.zaxnlqcnf.firm.in", "zaxnlqcnf.firm.in"},
  {"lrqmyr.epg.gubqkrf.lombardia.it", "gubqkrf.lombardia.it"},
  {"matsushima.miyagi.jp", NULL},
  {"khpr.fcnkseq.pbxtltxwn.valle-aosta.it", "pbxtltxwn.valle-aosta.it"},
  {"fjq.org.tw", "fjq.org.tw"},
  {".carrier.museum", NULL},
  {"qirkma.af.navy", "af.navy"},
  {".clmrd.wqklpyvq.kz", NULL},
  {"ymcjv.nom.pl", "ymcjv.nom.pl"},
  {"d..knhabj.homedepot", NULL},
  {"stsgeyd.b.kui.oketo.hokkaido.jp", "kui.oketo.hokkaido.jp"},
  {"tax", NULL},
  {"gulen.no", NULL},
  {"me.fcef.cxozvgld.co.ao", "cxozvgld.co.ao"},
  {"tab", NULL},
  {"lhjuuchi.wqbq.ooshika.nagano.jp", "wqbq.ooshika.nagano.jp"},
  {"quh.howkgxih.tamamura.gunma.jp", "howkgxih.tamamura.gunma.jp"},
  {"ymxazirgoq.is-uberleet.com", "ymxazirgoq.is-uberleet.com"},
  {"ldohgcet.fiuhxopesz.exety.álaheadju.no", "exety.álaheadju.no"},
  {"qkslqhue.gov.rs", "qkslqhue.gov.rs"},
  {"dxxhznk.capi.firebaseapp.com", "capi.firebaseapp.com"},
  {"org.pr", NULL},
  {"iygkecpn.xqyzxdxllg.gov.mg", "xqyzxdxllg.gov.mg"},
  {"theater.museum", NULL},
  {"chernihiv.ua", NULL},
  {".cfagnqhq.nirasaki.yamanashi.jp", NULL},
  {"lcluyvbe.npocansut.shinjo.okayama.jp", "npocansut.shinjo.okayama.jp"},
  {"cjqm.johana.toyama.jp", "cjqm.johana.toyama.jp"},
  {"rlnv.ualu.mamurogawa.yamagata.jp", "ualu.mamurogawa.yamagata.jp"},
  {"mll.bsikse.shinanomachi.nagano.jp", "bsikse.shinanomachi.nagano.jp"},
  {"zb.hr.komono.mie.jp", "hr.komono.mie.jp"},
  {"likescandy.com", NULL},
  {"co.tz", NULL},
  {"liomkat.fvbrczyh.xajymveoh.tj.cn", "xajymveoh.tj.cn"},
  {"wcdjljfb.ft.kishiwada.osaka.jp", "ft.kishiwada.osaka.jp"},
  {"jar.ru", NULL},
  {"dvry.u.skanland.no", "u.skanland.no"},
  {"bchiuwa.lkyogf.intl.tn", "lkyogf.intl.tn"},
  {"e.veax.nmmwlofqom.nakagawa.tokushima.jp", "nmmwlofqom.nakagawa.tokushima.jp"},
  {"kgvr.bådåddjå.no", "kgvr.bådåddjå.no"},
  {"uyxxxjw.wbcyw.nswaxiwfed.net.al", "nswaxiwfed.net.al"},
  {".su", NULL},
  {"zsnhpbqda.dk", "zsnhpbqda.dk"},
  {"lig.it", NULL},
  {"uymz.wknzzpk.zulnajeng.doesntexist.com", "zulnajeng.doesntexist.com"},
  {"tstwv..toyosato.shiga.jp", NULL},
  {"nuolw.org.sc", "nuolw.org.sc"},
  {"y.ballangen.no", "y.ballangen.no"},
  {".db.co.cr", NULL},
  {"camp", NULL},
  {"lnaokn.ux.oyslqow.jewelry.museum", "oyslqow.jewelry.museum"},
  {"ohtawara.tochigi.jp", NULL},
  {".xj.z.valle-d-aosta.it", NULL},
  {"b.rindal.no", "b.rindal.no"},
  {"qhrsaajys.jwde.rikubetsu.hokkaido.jp", "jwde.rikubetsu.hokkaido.jp"},
  {"xigidv.dz.msialsxtrt.soundandvision.museum", "msialsxtrt.soundandvision.museum"},
  {"jujgspla.ehrzjprfy.ewl.blue", "ewl.blue"},
  {"crimea.ua", NULL},
  {"lza.pbmhluoya.ztsw.hitachi", "ztsw.hitachi"},
  {"g.asahi.ibaraki.jp", "g.asahi.ibaraki.jp"},
  {"nishimera.miyazaki.jp", NULL},
  {"nausykmfl.epson", "nausykmfl.epson"},
  {"atgs..tottori.tottori.jp", NULL},
  {"..xycipxhfco.agents.aero", NULL},
  {"xsbtcpciyw.yokohama.jp", NULL},
  {"jdfjptcva.nvbt.ajjaetfl.pol.tr", "ajjaetfl.pol.tr"},
  {"me.njagbprg..iide.yamagata.jp", NULL},
  {"blog.br", NULL},
  {"rojnfhesdw.jos..snaase.no", NULL},
  {"prato.it", NULL},
  {"zirwofktz.moåreke.no", "zirwofktz.moåreke.no"},
  {"tbiqgqib.nopgdhrgi.kule.lib.vi.us", "kule.lib.vi.us"},
  {"veneto.it", NULL},
  {"vxnx.thn.tjhblqb.mizuho.tokyo.jp", "tjhblqb.mizuho.tokyo.jp"},
  {"gb.cjm.oaejzfes.kitayama.wakayama.jp", "oaejzfes.kitayama.wakayama.jp"},
  {"puujcj.takaishi.osaka.jp", "puujcj.takaishi.osaka.jp"},
  {"lpfazpf.oofwya.nnqtrgyost.togliatti.su", "nnqtrgyost.togliatti.su"},
  {"pgbo.cydpcylxp.otoyo.kochi.jp", "cydpcylxp.otoyo.kochi.jp"},
  {"nhaklnwvd.ovh", "nhaklnwvd.ovh"},
  {"g.佐賀.jp", "g.佐賀.jp"},
  {"geisei.kochi.jp", NULL},
  {"kse.k.wv.us", "k.wv.us"},
  {"kmluthcym.brussel.museum", "kmluthcym.brussel.museum"},
  {"bagnwvx.bvvv.ar.com", "bvvv.ar.com"},
  {"xfi.zb.avoues.fr", "zb.avoues.fr"},
  {".rf.onjs.net.gr", NULL},
  {".lrfzzlzb.endofinternet.net", NULL},
  {"qfaxgxult..p.rip", NULL},
  {".imjmaoek.kasumigaura.ibaraki.jp", NULL},
  {"vjk.cnkpqvvgzh.tomsk.ru", "cnkpqvvgzh.tomsk.ru"},
  {"zptsmb.lvh.vc.herøy.møre-og-romsdal.no", "vc.herøy.møre-og-romsdal.no"},
  {".qyidxymm.cffa.austrheim.no", NULL},
  {"bslevpb.plbpi.qpxb.ushiku.ibaraki.jp", "qpxb.ushiku.ibaraki.jp"},
  {"bbbzxnha.bjzxckaa.minato.tokyo.jp", "bjzxckaa.minato.tokyo.jp"},
  {".edu.pl", NULL},
  {"cfl.fsleknayh.info.au", "fsleknayh.info.au"},
  {"so.ypf.l.mazury.pl", "l.mazury.pl"},
  {".fjttiktq.nyc.museum", NULL},
  {"wkwvlqtu..wa.au", NULL},
  {"xsydxfwo.nakagawa.tokushima.jp", "xsydxfwo.nakagawa.tokushima.jp"},
  {"c.sbatklfi.toyone.aichi.jp", "sbatklfi.toyone.aichi.jp"},
  {"wyvh.zjxunmcdxy.cza.dominic.ua", "cza.dominic.ua"},
  {"fxko.yermh.doomdns.org", "yermh.doomdns.org"},
  {"ulan-ude.ru", NULL},
  {"clinxnma.unymiuwpit.hdkg.kosa.kumamoto.jp", "hdkg.kosa.kumamoto.jp"},
  {"meme", NULL},
  {"zbgerj.lixil", "zbgerj.lixil"},
  {"..prnkp.brussel.museum", NULL},
  {"www.fqzdmczrck.北海道.jp", "fqzdmczrck.北海道.jp"},
  {"barreau.bj", NULL},
  {".nic.in", NULL},
  {"wmloulb.c..naturhistorisches.museum", NULL},
  {"jsuxh.guykl.info.nf", "guykl.info.nf"},
  {"hwmncypk.gdx.kamagaya.chiba.jp", "gdx.kamagaya.chiba.jp"},
  {"gusx.qmuomjqh.zxxryy.medizinhistorisches.museum", "zxxryy.medizinhistorisches.museum"},
  {"wtvios.lfdpg.arrnopkysc.val-d-aosta.it", "arrnopkysc.val-d-aosta.it"},
  {"gm.lombardy.it", "gm.lombardy.it"},
  {".lt.n.dovre.no", NULL},
  {"uxrolfg.ruk.cthglr.flights", "cthglr.flights"},
  {"championship.aero", NULL},
  {"skei.rbi.ffzj.yuza.yamagata.jp", "ffzj.yuza.yamagata.jp"},
  {"trentino-stirol.it", NULL},
  {"kneeehbqxo.bo.im", "bo.im"},
  {"f.nrdyw.o.rochester.museum", "o.rochester.museum"},
  {"p.fuoisku.no", "p.fuoisku.no"},
  {"cb.fb.pharmacien.fr", "fb.pharmacien.fr"},
  {"m.ksizz.watch", "ksizz.watch"},
  {".kawara.fukuoka.jp", NULL},
  {"qxnhe.sabae.fukui.jp", "qxnhe.sabae.fukui.jp"},
  {"exposed", NULL},
  {"pgmbmvzgx.gw.kcvkiowmmo.living.museum", "kcvkiowmmo.living.museum"},
  {"co.uk", NULL},
  {"crr.lrfqbvg.pjt.otsuki.yamanashi.jp", "pjt.otsuki.yamanashi.jp"},
  {"trp.jtcmqep.oirase.aomori.jp", "jtcmqep.oirase.aomori.jp"},
  {"twepnmegnm.qn..tara.saga.jp", NULL},
  {"lyeqdmik.sakai.ibaraki.jp", "lyeqdmik.sakai.ibaraki.jp"},
  {"bxpff..xiivntu.ap-northeast-1.compute.amazonaws.com", NULL},
  {"lknwwrs.zstsugnd.net.mk", "zstsugnd.net.mk"},
  {"edu.bb", NULL},
  {"jhdhbupyy.kgbzjzzsp.fe.it", "kgbzjzzsp.fe.it"},
  {"fsunqqkndo.rrgqlyp.twpwyxluom.lindas.no", "twpwyxluom.lindas.no"},
  {"pa.gov.pl", NULL},
  {"zlf.de.soc.lk", "de.soc.lk"},
  {"kbuwnvqn.tcu.trentino.it", "tcu.trentino.it"},
  {"apsoyy.nyjibb.edu.ve", "nyjibb.edu.ve"},
  {"nja.groks-this.info", "nja.groks-this.info"},
  {"i.fwzwb.qmlvkanj.tm.pl", "qmlvkanj.tm.pl"},
  {"pars", NULL},
  {"okuizumo.shimane.jp", NULL},
  {"zkebqulhbu.shiiba.miyazaki.jp", "zkebqulhbu.shiiba.miyazaki.jp"},
  {"wbhgy.aoxdppxrb.bi.it", "aoxdppxrb.bi.it"},
  {".kawanabe.kagoshima.jp", NULL},
  {"neoadewo.nissedal.no", "neoadewo.nissedal.no"},
  {"sabae.fukui.jp", NULL},
  {"cpcccbouh.ldhjdcyd.in.th", "ldhjdcyd.in.th"},
  {"vzwbnoltsu.yt.balestrand.no", "yt.balestrand.no"},
  {"rkqcogmmxp.xeio.ajbwrvw.kiyosato.hokkaido.jp", "ajbwrvw.kiyosato.hokkaido.jp"},
  {"lu.iz.fjllfx.hamaroy.no", "fjllfx.hamaroy.no"},
  {"u.za.net", "u.za.net"},
  {"zwb..blogspot.ca", NULL},
  {"gqfayic.nfe.tokashiki.okinawa.jp", "nfe.tokashiki.okinawa.jp"},
  {".yqhfqh.lezajsk.pl", NULL},
  {"uauegmyck.pharmaciens.km", "uauegmyck.pharmaciens.km"},
  {"otznihai.gfx.ibm", "gfx.ibm"},
  {"yroyvn.vpzhubh.network", "vpzhubh.network"},
  {"kvjjos.gov.rs", "kvjjos.gov.rs"},
  {"chiropractic.museum", NULL},
  {"wiro.monza-e-della-brianza.it", "wiro.monza-e-della-brianza.it"},
  {"hwqvhr.snaase.no", "hwqvhr.snaase.no"},
  {"acan.mkplzg.indianapolis.museum", "mkplzg.indianapolis.museum"},
  {"vi", NULL},
  {"inderøy.no", NULL},
  {"h.bg", NULL},
  {"br.com", NULL},
  {"ywf..science", NULL},
  {"miasta.pl", NULL},
  {"شبكة", NULL},
  {"vvgefbtw.xzhjbntgmb.qa", "xzhjbntgmb.qa"},
  {"daqmkmg.bz.it", "daqmkmg.bz.it"},
  {"schweiz.museum", NULL},
  {"hum.trani-andria-barletta.it", "hum.trani-andria-barletta.it"},
  {".xulo.kamkdmfmlv.uzhgorod.ua", NULL},
  {"yqltxxaxkj.nisshin.aichi.jp", "yqltxxaxkj.nisshin.aichi.jp"},
  {"pfcuus.n.sorum.no", "n.sorum.no"},
  {"tmygvrt.hz.motorcycle.museum", "hz.motorcycle.museum"},
  {"izeybw.hd.gratis", "hd.gratis"},
  {"jeonnam.kr", NULL},
  {"mfqfgxn.twmgsfpi.tvvd.hazu.aichi.jp", "tvvd.hazu.aichi.jp"},
  {"qqxp.foit.gov.af", "foit.gov.af"},
  {"l.gp.vhkdysu.go.pw", "vhkdysu.go.pw"},
  {"susiz.x.org.ve", "x.org.ve"},
  {"chita.ru", NULL},
  {"mez.onga.fukuoka.jp", "mez.onga.fukuoka.jp"},
  {"a..mil.tz", NULL},
  {"jprvlxguz.wzgvzn.lib.nh.us", "wzgvzn.lib.nh.us"},
  {"xmyzavav..isumi.chiba.jp", NULL},
  {"kvjrb.संगठन", "kvjrb.संगठन"},
  {"r.ccemuc.wanouchi.gifu.jp", "ccemuc.wanouchi.gifu.jp"},
  {"flzvzpk.tst.jinsekikogen.hiroshima.jp", "tst.jinsekikogen.hiroshima.jp"},
  {"sardinia.it", NULL},
  {"tvspfz...kuzumaki.iwate.jp", NULL},
  {"k12.la.us", NULL},
  {"q..sex.hu", NULL},
  {"yvyorfgz.qqua.cleaning", "qqua.cleaning"},
  {"vjpide.dgtp.clcxxaunng.historical.museum", "clcxxaunng.historical.museum"},
  {"h.ziq.phurdgkswr.convent.museum", "phurdgkswr.convent.museum"},
  {"xkzf.lib.sc.us", "xkzf.lib.sc.us"},
  {"vipcyfac.nevqy.qijtif.k12.mi.us", "qijtif.k12.mi.us"},
  {"ckkeprkvp.ar.it", "ckkeprkvp.ar.it"},
  {"igkfly.frosinone.it", "igkfly.frosinone.it"},
  {"ir.r.uruma.okinawa.jp", "r.uruma.okinawa.jp"},
  {"lkxafongor.qnau..oshino.yamanashi.jp", NULL},
  {"yandex", NULL},
  {"ddqvpwhcqc.bzlgxnti.wouzeucoz.trd.br", "wouzeucoz.trd.br"},
  {"tatebayashi.gunma.jp", NULL},
  {"pzn.komaki.aichi.jp", "pzn.komaki.aichi.jp"},
  {"tzwdoxesmy.xnr.v.hirono.fukushima.jp", "v.hirono.fukushima.jp"},
  {"orkdal.no", NULL},
  {"kco.furulzzq..vercelli.it", NULL},
  {".n.historichouses.museum", NULL},
  {"c.bn.ctzl.kaita.hiroshima.jp", "ctzl.kaita.hiroshima.jp"},
  {"wvvzuxwvtm.a.bg", "wvvzuxwvtm.a.bg"},
  {"upcsbiont.kasama.ibaraki.jp", "upcsbiont.kasama.ibaraki.jp"},
  {"ytwn.y.tm.maryland.museum", "tm.maryland.museum"},
  {"czcig.sexy", "czcig.sexy"},
  {"com.vc", NULL},
  {"tsdalw.h.boldlygoingnowhere.org", "h.boldlygoingnowhere.org"},
  {"jryscl.kgl.heroy.nordland.no", "kgl.heroy.nordland.no"},
  {"mvb.rsvp", "mvb.rsvp"},
  {"plrmi.kxlsy.rzpx.hirata.fukushima.jp", "rzpx.hirata.fukushima.jp"},
  {"castle.museum", NULL},
  {".tgory.pl", NULL},
  {"kdvvyevzuw.x.uamsoow.isernia.it", "uamsoow.isernia.it"},
  {"uil.yqzojogux.org.st", "yqzojogux.org.st"},
  {"p.ksottc.gifu.jp", "ksottc.gifu.jp"},
  {"vcs.noboribetsu.hokkaido.jp", "vcs.noboribetsu.hokkaido.jp"},
  {"u.ikeda.gifu.jp", "u.ikeda.gifu.jp"},
  {"imisrgrpr.e.cqzza.org.vn", "cqzza.org.vn"},
  {"viva", NULL},
  {"ap.bp.mashike.hokkaido.jp", "bp.mashike.hokkaido.jp"},
  {"cc.mt.us", NULL},
  {"mcts..b.bg", NULL},
  {"nord-odal.no", NULL},
  {"byrl.org.st", "byrl.org.st"},
  {"fi.cr", NULL},
  {"xmsdv.pdmxgxnq.v.vestre-slidre.no", "v.vestre-slidre.no"},
  {"cdtp.sjmedqk.ckibqhbwv.uchiko.ehime.jp", "ckibqhbwv.uchiko.ehime.jp"},
  {"fdl.oqqld.mi.wiki.br", "mi.wiki.br"},
  {"yrzqpzakw.certification.aero", "yrzqpzakw.certification.aero"},
  {"gniatu.zc.pcztfnlz.kyuragi.saga.jp", "pcztfnlz.kyuragi.saga.jp"},
  {"gs.ah.no", NULL},
  {"relmgxu.ejvafdehbl.info.la", "ejvafdehbl.info.la"},
  {"pauseipc.lvcxdudnfw.oqotv.co.com", "oqotv.co.com"},
  {"iglesiascarbonia.it", NULL},
  {"wdhbhbqsf.yudsgohoz.a.higashisumiyoshi.osaka.jp", "a.higashisumiyoshi.osaka.jp"},
  {"nm.name.qa", "nm.name.qa"},
  {"cafdzmy.test.ru", "cafdzmy.test.ru"},
  {"fareast.ru", NULL},
  {"kiwi", NULL},
  {"mhlqqu.wgyzypxkiw.com.ws", "wgyzypxkiw.com.ws"},
  {"oqlprf.mq.yp.desi", "yp.desi"},
  {"nf.pil..shimamoto.osaka.jp", NULL},
  {"works", NULL},
  {"troitsk.su", NULL},
  {"rw.xpturnw.woxuwkadzf.toyama.toyama.jp", "woxuwkadzf.toyama.toyama.jp"},
  {"civilization.museum", NULL},
  {"ing.pa", NULL},
  {"wyvc.tzugfbs.bungotakada.oita.jp", "tzugfbs.bungotakada.oita.jp"},
  {"xiqwrwqx.j.mp.br", "j.mp.br"},
  {"umbria.it", NULL},
  {"ivdhid.tdy.zzbdvb.nichinan.miyazaki.jp", "zzbdvb.nichinan.miyazaki.jp"},
  {"wwwj.岡山.jp", "wwwj.岡山.jp"},
  {".pn.com.vu", NULL},
  {"nepjl.bosfgsty.yebefm.sasayama.hyogo.jp", "yebefm.sasayama.hyogo.jp"},
  {".tfbl.kashihara.nara.jp", NULL},
  {"skaxh.finance", "skaxh.finance"},
  {"tm.hu", NULL},
  {".matsusaka.mie.jp", NULL},
  {"delivery", NULL},
  {"erotika.hu", NULL},
  {"zkagc.bhywnzxcn.nnx.pyatigorsk.ru", "nnx.pyatigorsk.ru"},
  {"bbgwypp.rdoeiyzyt.am.stjohn.museum", "am.stjohn.museum"},
  {"isyh.fk", NULL},
  {"...shichikashuku.miyagi.jp", NULL},
  {"mw.historisch.museum", "mw.historisch.museum"},
  {"fpe..aya.miyazaki.jp", NULL},
  {"folldal.no", NULL},
  {"uiebeixbfx.ixkflnil.realty", "ixkflnil.realty"},
  {"ad.xgmj.radom.pl", "xgmj.radom.pl"},
  {"nvkbzqie.kyoto", "nvkbzqie.kyoto"},
  {"mizumaki.fukuoka.jp", NULL},
  {"rcvdtle.room", "rcvdtle.room"},
  {"unwjadhwuf.yashiro.hyogo.jp", "unwjadhwuf.yashiro.hyogo.jp"},
  {"togura.nagano.jp", NULL},
  {"qfvd.warszawa.pl", "qfvd.warszawa.pl"},
  {"akagi.shimane.jp", NULL},
  {"fan", NULL},
  {"encm.clrioeitd.net.je", "clrioeitd.net.je"},
  {"sec.ps", NULL},
  {"kamisu.ibaraki.jp", NULL},
  {"udzh..csodh.br", NULL},
  {"nycay.sibenik.museum", "nycay.sibenik.museum"},
  {"wmfdmk.aexbp.jrmsunoj.happou.akita.jp", "jrmsunoj.happou.akita.jp"},
  {"hinode.tokyo.jp", NULL},
  {"trentinosud-tirol.it", NULL},
  {"ndmuz.injgl.pw.cri.nz", "pw.cri.nz"},
  {"bgpdzistsy.npyuk.gs.hm.no", "npyuk.gs.hm.no"},
  {"verona.it", NULL},
  {".net.nz", NULL},
  {"opfjc.tzbdwvfjpj.xqjuxqdc.ina.nagano.jp", "xqjuxqdc.ina.nagano.jp"},
  {"emqmz.t.usutsaqex.nahari.kochi.jp", "usutsaqex.nahari.kochi.jp"},
  {"xz..qhd.zhytomyr.ua", NULL},
  {"vjqi.chelyabinsk.ru", "vjqi.chelyabinsk.ru"},
  {"ugrphc.jp", "ugrphc.jp"},
  {"ufxcph.npfx.ljiafzx.place", "ljiafzx.place"},
  {"do..olsztyn.pl", NULL},
  {"tochio.niigata.jp", NULL},
  {"vchp.net.hn", "vchp.net.hn"},
  {"suuh.jero.nb.naha.okinawa.jp", "nb.naha.okinawa.jp"},
  {"gawnsc.cc.ar.us", "gawnsc.cc.ar.us"},
  {"uxudpv.nd.certification.aero", "nd.certification.aero"},
  {"rffrqo.sjg.hofu.yamaguchi.jp", "sjg.hofu.yamaguchi.jp"},
  {"trjfmzaox.shirakawa.fukushima.jp", "trjfmzaox.shirakawa.fukushima.jp"},
  {"ergsb.o.urn.arpa", "o.urn.arpa"},
  {"kkjp.gxpohvyczo.com.pt", "gxpohvyczo.com.pt"},
  {"fly", NULL},
  {"v.mvy.zg.gmo", "zg.gmo"},
  {"mq", NULL},
  {"zitlvyte.qq.med.pa", "qq.med.pa"},
  {"ozodmwnfcs.unyg.minamiashigara.kanagawa.jp", "unyg.minamiashigara.kanagawa.jp"},
  {"fxivgowsad.mvjhmi..bv.nl", NULL},
  {"oa.tesqe.zb.suldal.no", "zb.suldal.no"},
  {"xifvoam.tmgo.info.tr", "tmgo.info.tr"},
};

static void
test_public_suffix (const char *host, const char *exp)
{
  struct url url = {0};
  char *str;
  const char *out;

  assert (asprintf (&str, "http://%s/", host) >= 0);
  assert (url_parse (&url, str) == 0);

  out = url_public_suffix (&url);
  assert ((out != NULL) == (exp != NULL));
  if (exp)
    assert (strcmp (out, exp) == 0);
  free (str);
}

int
main (void)
{
  size_t n = sizeof (cases) / sizeof (cases[0]);
  size_t i;
  for (i = 0; i < n; i++) {
    puts (cases[i].inp);
    test_public_suffix (cases[i].inp, cases[i].exp);
  }
  return 0;
}
