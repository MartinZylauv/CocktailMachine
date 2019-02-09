using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Http;
using System.Text;
using Android;
using Android.App;
using Android.Bluetooth;
using Android.OS;
using Android.Runtime;
using Android.Support.Design.Widget;
using Android.Support.V4.View;
using Android.Support.V4.Widget;
using Android.Support.V7.App;
using Android.Views;
using Android.Widget;
using Java.Lang;
using Java.Util;
using Newtonsoft.Json;
using System.Linq;

using Exception = System.Exception;
using static Android.App.ActionBar;
using Android.Graphics;
using Java.Net;
using Java.IO;
using System.Net;
using Android.Util;

namespace Drinks
{
    [Activity(Label = "@string/app_name", Theme = "@style/AppTheme.NoActionBar", MainLauncher = true)]
    public class MainActivity : AppCompatActivity, NavigationView.IOnNavigationItemSelectedListener
    {

        BluetoothAdapter adapter = BluetoothAdapter.DefaultAdapter;
        BluetoothSocket _socket = null;

        protected override void OnCreate(Bundle savedInstanceState)
        {
            base.OnCreate(savedInstanceState);
            SetContentView(Resource.Layout.activity_main);
            Android.Support.V7.Widget.Toolbar toolbar = FindViewById<Android.Support.V7.Widget.Toolbar>(Resource.Id.toolbar);
            SetSupportActionBar(toolbar);

            FloatingActionButton fab = FindViewById<FloatingActionButton>(Resource.Id.fab);
            fab.Click += FabOnClick;

            DrawerLayout drawer = FindViewById<DrawerLayout>(Resource.Id.drawer_layout);
            ActionBarDrawerToggle toggle = new ActionBarDrawerToggle(this, drawer, toolbar, Resource.String.navigation_drawer_open, Resource.String.navigation_drawer_close);
            drawer.AddDrawerListener(toggle);
            toggle.SyncState();

            NavigationView navigationView = FindViewById<NavigationView>(Resource.Id.nav_view);
            navigationView.SetNavigationItemSelectedListener(this);


            //ImageButton button2 = FindViewById<ImageButton>(Resource.Id.button2);

            //ImageButton button3 = FindViewById<ImageButton>(Resource.Id.button3);

            //moscowmule



            ConnectToBTAsync();
            //getting the drinks from the webserver
            DrinksJson drinks = null;
            using (var client = new HttpClient())
            {

                //all the drinks are currently in aformat which can be seen in the below url. You can change this url to a path of a file, or your own website so the app can get new drinks all the time! :)
                client.BaseAddress = new Uri("http://zylauv.dk/drinks/");
                var response = client.GetAsync("drinks.json");

                var drinksJson = response.Result?.Content?.ReadAsStringAsync()?.Result;

                drinks = JsonConvert.DeserializeObject<DrinksJson>(drinksJson);
            }

            var main = FindViewById<LinearLayout>(Resource.Id.main);
            var count = 0;
            var currentLinear = new LinearLayout(this);
            main.AddView(currentLinear);
            foreach (var drink in drinks.Drinks)
            {
                count++;

                if ((count % 3) == 0)
                {
                    currentLinear = GetLinearLayout();
                    main.AddView(currentLinear);
                    count = 1;

                }

                var btn = GetImageButton(drink.ImgUrl);
                currentLinear.AddView(btn);


                btn.Click += (o, e) =>
                {

                    if (_socket != null)
                        MakeDrink(drink);
                    else
                    {
                        ConnectToBTAsync();
                    }
                };


            }
            

        }



        private async System.Threading.Tasks.Task MakeDrink(Drink drink)
        {

            //// Read data from the device
            //await _socket.InputStream.ReadAsync(buffer, 0, buffer.Length);

            // Write data to the device

            var cmdName = "MAKE_DRINK";
            var cmdString = $"$cmd:{cmdName};";



            var bas = new List<BottleAmount>();

            foreach (var ba in drink.BottleAmounts)
            {
                bas.Add(ba);
            }

            foreach (var b in bas)
            {
                cmdString += $"btl:{b.bottleNum},{b.amount};";
            }

            cmdString += "$";


            byte[] bytes = Encoding.UTF8.GetBytes(cmdString);


            int skip = 0;
            while (skip != bytes.Count())
            {

                var by = bytes.Skip(skip).Take(10).ToArray();
                await _socket.OutputStream.WriteAsync(by, 0, by.Count());

                skip += by.Count();
            }


        }
        
        private async System.Threading.Tasks.Task ConnectToBTAsync()
        {
            if (adapter == null)
                Toast.MakeText(this, "No bt adapter found", ToastLength.Short).Show();

            if (!adapter.IsEnabled)
                Toast.MakeText(this, "bt adapter not enabled", ToastLength.Short).Show();


            BluetoothDevice device = (from bd in adapter.BondedDevices
                                      where bd.Name == "HC-05"
                                      select bd).FirstOrDefault();

            if (device == null)
                Toast.MakeText(this, "no device found...", ToastLength.Short).Show();

            _socket = device.CreateRfcommSocketToServiceRecord(UUID.FromString("00001101-0000-1000-8000-00805f9b34fb"));

            try
            {
                await _socket.ConnectAsync();

            }
            catch (Exception e)
            {

                Toast.MakeText(this, "could not connect", ToastLength.Short).Show();
            }

        }
        public override void OnBackPressed()
        {
            DrawerLayout drawer = FindViewById<DrawerLayout>(Resource.Id.drawer_layout);
            if (drawer.IsDrawerOpen(GravityCompat.Start))
            {
                drawer.CloseDrawer(GravityCompat.Start);
            }
            else
            {
                base.OnBackPressed();
            }
        }
        public override bool OnCreateOptionsMenu(IMenu menu)
        {
            MenuInflater.Inflate(Resource.Menu.menu_main, menu);
            return true;
        }

        public override bool OnOptionsItemSelected(IMenuItem item)
        {
            int id = item.ItemId;
            if (id == Resource.Id.action_settings)
            {
                return true;
            }

            return base.OnOptionsItemSelected(item);
        }

        private void FabOnClick(object sender, EventArgs eventArgs)
        {
            View view = (View)sender;
            Snackbar.Make(view, "Replace with your own action", Snackbar.LengthLong)
                .SetAction("Action", (Android.Views.View.IOnClickListener)null).Show();
        }


        private LinearLayout GetLinearLayout()
        {
            LinearLayout current = new LinearLayout(this);
            LinearLayout.LayoutParams linearLayoutParams = new LinearLayout.LayoutParams(LinearLayout.LayoutParams.FillParent, LinearLayout.LayoutParams.WrapContent);
            current.LayoutParameters = linearLayoutParams;
            current.WeightSum = 1;
            current.Orientation = Orientation.Horizontal;


            return current;
        }

        private ImageButton GetImageButton(string imgUrl)
        {

            ImageButton btn = new ImageButton(this);
            LinearLayout.LayoutParams layoutParams = new LinearLayout.LayoutParams(0, 200, 0.5f);

            btn.SetScaleType(ImageView.ScaleType.CenterCrop);
            layoutParams.Gravity = GravityFlags.Center;
            btn.SetBackgroundColor(Android.Graphics.Color.Transparent);
            var imageBitmap = GetImageBitmapFromUrl(imgUrl);
            //var dp = 300;
            //int pixel = (int)TypedValue.ApplyDimension(ComplexUnitType.Dip, dp, this.Resources.DisplayMetrics);

            //if (imageBitmap.Height < pixel)
            //{

            //    var width = GetWidth(imageBitmap.Width, imageBitmap.Height, 300);
            //    imageBitmap = Bitmap.CreateScaledBitmap(imageBitmap, width, 300, true);
            //}



            btn.SetImageBitmap(imageBitmap);
            btn.LayoutParameters = layoutParams;






            return btn;
        }


        private int GetWidth(int width, int height, int newHeight)
        {

            var ratio = width / height;

            return newHeight / ratio;



        }

        public bool OnNavigationItemSelected(IMenuItem item)
        {
            int id = item.ItemId;

            if (id == Resource.Id.nav_camera)
            {
                // Handle the camera action
            }
            else if (id == Resource.Id.nav_gallery)
            {

            }
            else if (id == Resource.Id.nav_slideshow)
            {

            }
            else if (id == Resource.Id.nav_manage)
            {

            }
            else if (id == Resource.Id.nav_share)
            {

            }
            else if (id == Resource.Id.nav_send)
            {

            }

            DrawerLayout drawer = FindViewById<DrawerLayout>(Resource.Id.drawer_layout);
            drawer.CloseDrawer(GravityCompat.Start);
            return true;
        }



        private Bitmap GetImageBitmapFromUrl(string url)
        {
            Bitmap imageBitmap = null;

            using (var webClient = new WebClient())
            {
                var imageBytes = webClient.DownloadData(url);
                if (imageBytes != null && imageBytes.Length > 0)
                {
                    imageBitmap = BitmapFactory.DecodeByteArray(imageBytes, 0, imageBytes.Length);
                }
            }

            return imageBitmap;
        }
    }



    public class Drink
    {
        public string Name { get; set; }

        public string ImgUrl { get; set; }
        public List<BottleAmount> BottleAmounts { get; set; }
    }

    public class DrinksJson
    {
        public List<Drink> Drinks { get; set; }
    }


    public class BottleAmount
    {
        public int bottleNum { get; set; }
        public int amount { get; set; }
    }
}

